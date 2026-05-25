@echo off
MSBuild /nologo libsimulex.sln /Target:Build /Property:Configuration=Debug /Property:Platform=Win32
MSBuild /nologo libsimulex.sln /Target:Build /Property:Configuration=Debug /Property:Platform=x64
MSBuild /nologo libsimulex.sln /Target:Build /Property:Configuration=Release /Property:Platform=Win32
MSBuild /nologo libsimulex.sln /Target:Build /Property:Configuration=Release /Property:Platform=x64
