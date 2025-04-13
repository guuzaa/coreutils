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

    fn build(self: Executable, b: *std.Build, optimize: std.builtin.Mode, target: std.Build.ResolvedTarget, targets: *std.ArrayList(*std.Build.Step.Compile)) void {
        const exe = b.addExecutable(.{
            .name = self.name,
            .target = target,
            .optimize = optimize,
        });
        targets.append(exe) catch @panic("OOM");
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
    var targets = std.ArrayList(*std.Build.Step.Compile).init(b.allocator);
    defer targets.deinit();

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

    zcc.createStep(b, "cdb", targets.toOwnedSlice() catch @panic("OOM"));
}
