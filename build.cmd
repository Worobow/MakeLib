
set target=build

MSBuild.exe /t:%target% /p:Configuration=Release /p:Platform=x64
pushd OutBin\x64\Release
del target.exe
ImageMerge2.exe TestEject.Exe TestEject.pdb TestExe.Exe TestExe.pdb  target.exe 1 >ImageMerge.log
target >target.log
popd

MSBuild.exe /t:%target% /p:Configuration=Release /p:Platform=Win32
pushd  OutBin\Win32\Release
del target.exe
ImageMerge2.exe TestEject.Exe TestEject.pdb TestExe.Exe TestExe.pdb  target.exe 1 >ImageMerge.log
target >target.log
popd

MSBuild.exe /t:%target% /p:Configuration=Debug /p:Platform=x64
pushd OutBin\x64\Debug
del target.exe
ImageMerge2.exe TestEject.Exe TestEject.pdb TestExe.Exe TestExe.pdb  target.exe 1 >ImageMerge.log
target >target.log
popd

MSBuild.exe /t:%target% /p:Configuration=Debug /p:Platform=Win32
pushd  OutBin\Win32\Debug
del target.exe
ImageMerge2.exe TestEject.Exe TestEject.pdb TestExe.Exe TestExe.pdb  target.exe 1 >ImageMerge.log
target >target.log
popd

