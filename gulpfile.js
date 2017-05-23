var gulp = require("gulp");
var clean = require("gulp-clean");
var exec = require("child_process").exec;
var buildCommand = "gcc src/*.c -o hoffman.out";
var execCommand = "./hoffman.out";

gulp.task("build",function(){
    exec(buildCommand, function (error, stdout, stderr) {
        console.log(stdout);
        console.log(stderr);
    });
});

gulp.task("test",function(){
    exec(execCommand, function (error, stdout, stderr) {
        console.log(stdout);
        console.log(stderr);
    });
});

gulp.task("clean",function(){
    gulp.src("bin", {read: false})
        .pipe(clean());
});

gulp.task("watch", function() {
    gulp.watch("src/*.c", ["build"]);
    gulp.watch("include/*.h", ["build"]);
});

gulp.task("default",["build","watch"],function(){});