### For linux install following packages
```
sudo apt update
sudo apt install libxrandr-dev libxcursor-dev libudev-dev libfreetype-dev libopenal-dev libflac-dev libvorbis-dev libgl1-mesa-dev libegl1-mesa-dev build-essential cmake libxi-dev libx11-dev
```

## Build

```
git clone https://github.com/Karan-Semwal/SFGame.git
cmake -S . -B build -G "Unix Makefiles"
cmake --build build
```
