param (
    [string]$build
)

$filename = "visualizer"

if (Test-Path "cpp/src/$filename.cpp") {
    switch ($build) {
        "build" {
            if (-not (Test-Path "cpp/build")) {
                mkdir "cpp/build"
            }
            g++ "cpp/src/$filename.cpp" "cpp/src/glad.c" "cpp/src/test_axes.cpp" -o "cpp/build/$filename.exe" -I "cpp/include" -L "cpp/lib" -lglew32 -lglfw3 -lopengl32 -lgdi32
        }
        "run" {
            try {
                Start-Process -NoNewWindow -FilePath "cpp/build/$filename.exe"
            }
            catch {
                Write-Output "Failed to run: Try building again before running"
            }
        }
        default {
            Write-Output "ValueError: Argument should only be 'build' or 'run'"
        }
    }
} else {
    Write-Output "NameError: File does not exist"
}
