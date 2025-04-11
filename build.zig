const std = @import("std");
const zcc = @import("compile_commands");

const executable_name = "wc";

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});
    var targets = std.ArrayList(*std.Build.Step.Compile).init(b.allocator);

    const exe = b.addExecutable(.{
        .name = executable_name,
        .target = target,
        .optimize = optimize,
    });
    targets.append(exe) catch @panic("OOM");

    const cpp_sources = [_][]const u8{
        "src/main.cpp",
        "src/wc.cpp",
        "src/options.cpp",
        "src/params.cpp",
    };

    for (cpp_sources) |source| {
        exe.addCSourceFile(.{
            .file = b.path(source),
            .flags = &[_][]const u8{"-std=c++20"},
        });
    }

    exe.linkLibCpp();
    exe.addIncludePath(b.path("src"));

    b.installArtifact(exe);

    const run_cmd = b.addRunArtifact(exe);
    run_cmd.step.dependOn(b.getInstallStep());

    if (b.args) |args| {
        run_cmd.addArgs(args);
    }

    const run_step = b.step("run", "Run the app");
    run_step.dependOn(&run_cmd.step);
    zcc.createStep(b, "cdb", targets.toOwnedSlice() catch @panic("OOM"));
}
