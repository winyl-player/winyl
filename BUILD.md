**DRAFT**

You need Visual Studio C++ 2017 (with Windows XP support for C++).

x64 build is broken. I'll fix it later.

First you need to compile all 3rd-party libraries.

----------
Build SQLite:

1. Download amalgamation from https://www.sqlite.org/download.html
2. Unzip all files to Winyl/src/sqlite3/sqlite3/src
3. Build Winyl/src/sqlite3/sqlite3.sln

----------
Build TagLib:

1. Download https://github.com/taglib/taglib
2. Configure Visual Studio projects with CMake (CMake GUI is the easiest way) with ENABLE_STATIC_RUNTIME only option
3. Patch file taglib/mpeg/id3v2/id3v2tag.cpp: in function downgradeFrames replace all String::Latin1 to String::UTF16 (must be 5 replacements)
4. In Visual studio in tag project properties change Platform Toolset to v141_xp
5. Build only tag project, other projects aren't needed
6. Copy all .h and .tcc files to Winyl/src/taglib (cmdline: for /r "taglib-master\taglib" %f in (*.h;*.tcc) do copy "%f" "Winyl\Winyl\src\taglib")
7. x86: Copy Release and Debug folders with .lib and .pdb files to Winyl/src/taglib
8. x64: Reconfigure Win64 Visual Studio projects with CMake and build it
9. x64: Copy Release and Debug folders with .lib and .pdb files to Winyl/src/taglib/x64

----------
Build zlib (can be skipped, needed only for PackSkin utility):

1. Download zlib https://zlib.net
2. Open contrib/vstudio/vc14/zlibvc.sln
3. Open zlibstat ReleaseWithoutAsm project properties
4. Change Platform Toolset to v141_xp
5. Change C/C++ -> Code Generation -> Runtime Library -> Set /MT
6. Change C/C++ -> General -> Debug Information Format -> Set None
7. Build zlibstat ReleaseWithoutAsm
8. x86: Copy zlibstat.lib to Winyl/src/zlib
9. x64: Copy zlibstat.lib to Winyl/src/zlib/x64
10. Copy zlib.h, zconf.h, ioapi.h, zip.h, unzip.h to Winyl/src/zlib
11. Build PackSkin utility Winyl/PackSkin/PackSkin.sln

----------
pugixml:

1. Download pugixml https://pugixml.org
2. Unzip it to Winyl/src/pugixml
3. Uncomment the following lines in pugiconfig.hpp  
#define PUGIXML_NO_XPATH  
#define PUGIXML_NO_STL  
#define PUGIXML_NO_EXCEPTIONS  
#define PUGIXML_HEADER_ONLY  
#define PUGIXML_HAS_LONG_LONG  

----------
BASS:

1. Download from http://www.un4seen.com  
bass24.zip  
bassmix24.zip  
bass_fx24.zip  
basswasapi24.zip  
bassasio13.zip  
bass_aac24.zip  
bass_ape24.zip  
bass_mpc24.zip  
bass_spx24.zip  
bass_tta24.zip  
bassalac24.zip  
bassflac24.zip  
bassopus24.zip  
basswm24.zip  
basswv24.zip  
2. Unzip .h files to Winyl/src/bass
3. x86: Unzip .lib files to Winyl/src/bass (for bass_fx use .lib file from safeseh folder)
4. x64: Unzip x64 .lib files to Winyl/src/bass/x64

----------
That is all if you only need to compile the release version, build it with Winyl.sln.

----------
To run under the debugger or create packages you need Winyl/data folder.

The data folder structure:

File.ico  
License.txt  
Portable.dat  
Equalizer/Presets.xml  
Language/*  
Skin/*  
x86/bass.dll  
x86/bass_fx.dll  
x86/bassasio.dll  
x86/bassmix.dll  
x86/basswasapi.dll  
x86/Bass/bass_aac.dll  
x86/Bass/bass_ape.dll  
x86/Bass/bass_mpc.dll  
x86/Bass/bass_spx.dll  
x86/Bass/bass_tta.dll  
x86/Bass/bassalac.dll  
x86/Bass/bassflac.dll  
x86/Bass/bassopus.dll  
x86/Bass/basswma.dll  
x86/Bass/basswv.dll  
x64/(the same as above but dlls are x64)  


----------
Creating packages:

1. Copy the data folder somewhere
2. Remove Profile subfolder
3. Copy Winyl.exe to the data folder
4. Move dlls from x86/x64 to the data folder and delete x86 and x64 folders
5. Copy PackSkin.exe to data\Skin folder
6. Pack all skins with PackSkin utility and delete unpacked skins
7. Rename the data folder to 'Winyl'
8. Portable version: zip the folder
9. Setup version: run Inno Setup script
10. Repeat all for x64 version

