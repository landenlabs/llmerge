#!/bin/csh -f

set app=llmerge
xcodebuild -list -project $app.xcodeproj

# rm -rf DerivedData/
xcodebuild -scheme $app -configuration Release clean build
# xcodebuild -configuration Release -alltargets clean

echo -----------------------------
find ./DerivedData -type f -name $app -perm +111 -ls
set src=./DerivedData/Build/Products/Release/$app

echo -----------------------------
echo "File ß$src"
ls -al $src
cp $src ~/opt/bin/