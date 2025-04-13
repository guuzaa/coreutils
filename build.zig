const std = @import("std");
const zcc = @import("compile_commands");


pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});
    var targets = std.ArrayList(*std.Build.Step.Compile).init(b.allocator);

    const wc_exe = b.addExecutable(.{
        .name = "wc",
        .target = target,
        .optimize = optimize,
    });
    targets.append(wc_exe) catch @panic("OOM");

    const cpp_sources = [_][]const u8{
        "src/wc/main.cpp",
        "src/wc/wc.cpp",
        "src/wc/options.cpp",
        "src/wc/params.cpp",
    };

    for (cpp_sources) |source| {
        wc_exe.addCSourceFile(.{
            .file = b.path(source),
            .flags = &[_][]const u8{"-std=c++20"},
        });
    }

    wc_exe.linkLibCpp();
    wc_exe.addIncludePath(b.path("src/wc"));

    b.installArtifact(wc_exe);

    const run_cmd = b.addRunArtifact(wc_exe);
    run_cmd.step.dependOn(b.getInstallStep());

    if (b.args) |args| {
        run_cmd.addArgs(args);
    }

    const run_step = b.step("run", "Run the app");
    run_step.dependOn(&run_cmd.step);
    zcc.createStep(b, "cdb", targets.toOwnedSlice() catch @panic("OOM"));
}
