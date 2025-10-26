const std = @import("std");
const zcc = @import("compile_commands");

const Executable = struct {
    name: []const u8,
    cpp_sources: []const []const u8,
    include_paths: []const []const u8,

    fn init(name: []const u8, cpp_sources: []const []const u8, include_paths: []const []const u8) Executable {
        return Executable{
            .name = name,
            .cpp_sources = cpp_sources,
            .include_paths = include_paths,
        };
    }

    fn build(self: Executable, b: *std.Build, optimize: std.builtin.OptimizeMode, target: std.Build.ResolvedTarget, targets: *std.ArrayList(*std.Build.Step.Compile)) void {
        const exe = b.addExecutable(.{ .name = self.name, .root_module = b.createModule(.{
            .target = target,
            .optimize = optimize,
        }) });

        for (self.cpp_sources) |source| {
            exe.addCSourceFile(.{
                .file = b.path(source),
                .flags = &[_][]const u8{"-std=c++20"},
            });
        }
        exe.linkLibCpp();
        for (self.include_paths) |include_path| {
            exe.addIncludePath(b.path(include_path));
        }
        b.installArtifact(exe);
        targets.append(b.allocator, exe) catch @panic("OOM");

        const run_cmd_name = std.fmt.allocPrint(b.allocator, "run-{s}", .{self.name}) catch @panic("OOM");
        const run_cmd_info = std.fmt.allocPrint(b.allocator, "Run the {s} app", .{self.name}) catch @panic("OOM");
        const run_step = b.step(run_cmd_name, run_cmd_info);
        const run_cmd = b.addRunArtifact(exe);
        run_cmd.step.dependOn(b.getInstallStep());

        if (b.args) |args| {
            run_cmd.addArgs(args);
        }
        run_step.dependOn(&run_cmd.step);
    }
};

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});
    var targets: std.ArrayList(*std.Build.Step.Compile) = .empty;
    defer targets.deinit(b.allocator);

    Executable.init("wc", &[_][]const u8{
        "src/wc/main.cpp",
        "src/wc/wc.cpp",
        "src/wc/options.cpp",
        "src/wc/params.cpp",
    }, &[_][]const u8{
        "src/wc",
    }).build(b, optimize, target, &targets);

    Executable.init("true", &[_][]const u8{
        "src/true/main.cpp",
    }, &[_][]const u8{
        "src/true",
    }).build(b, optimize, target, &targets);

    Executable.init("false", &[_][]const u8{
        "src/false/main.cpp",
    }, &[_][]const u8{
        "src/false",
    }).build(b, optimize, target, &targets);

    const googletest = b.dependency("googletest", .{
        .target = target,
        .optimize = optimize,
    });

    const gtest = b.addLibrary(.{ .name = "gtest", .linkage = .static, .root_module = b.createModule(.{
        .target = target,
        .optimize = optimize,
    }) });
    
    gtest.linkLibCpp();
    gtest.addCSourceFiles(
        .{
            .root = googletest.path("googletest/src"),
            .files = &.{
                "gtest-all.cc",
                "gtest-assertion-result.cc",
                "gtest-death-test.cc",
                "gtest-filepath.cc",
                "gtest-matchers.cc",
                "gtest-port.cc",
                "gtest-printers.cc",
                "gtest-test-part.cc",
                "gtest-typed-test.cc",
                "gtest.cc",
                "gtest_main.cc",
            },
            .flags = &.{
                "-std=c++17",
            },
        },
    );
    targets.append(b.allocator, gtest) catch @panic("OOM");

    gtest.addIncludePath(googletest.path("googletest"));
    gtest.addIncludePath(googletest.path("googletest/src"));
    gtest.addIncludePath(googletest.path("googletest/include"));
    gtest.addIncludePath(googletest.path("googletest/include/internal"));
    gtest.addIncludePath(googletest.path("googletest/include/internal/custom"));

    // Build test executables
    const options_test = b.addExecutable(.{ .name = "option_test", .root_module = b.createModule(.{
        .target = target,
        .optimize = optimize,
    }) });

    const options_test_cpp_sources = [_][]const u8{
        "tests/wc/options.test.cpp",
        "src/wc/options.cpp",
        "src/wc/params.cpp",
    };
    
    for (options_test_cpp_sources) |source| {
        options_test.addCSourceFile(.{
            .file = b.path(source),
            .flags = &[_][]const u8{"-std=c++20"},
        });
    }
    options_test.linkLibCpp();
    options_test.linkLibrary(gtest);
    options_test.addIncludePath(b.path("src/wc"));
    options_test.addIncludePath(googletest.path("googletest/include"));
    b.installDirectory(
        .{
            .source_dir = googletest.path("googletest/include"),
            .install_dir = .header,
            .install_subdir = "tests",
        },
    );
    b.installArtifact(options_test);
    const run_test = b.addRunArtifact(options_test);
    const run_test_step = b.step("test", "Run the tests");
    run_test_step.dependOn(&run_test.step);
    targets.append(b.allocator, options_test) catch @panic("OOM");

    _ = zcc.createStep(b, "cdb", targets.toOwnedSlice(b.allocator) catch @panic("OOM"));
}
