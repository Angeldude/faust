/************************************************************************
 FAUST Architecture File
 Copyright (C) 2018 GRAME, Centre National de Creation Musicale
 ---------------------------------------------------------------------
 This Architecture section is free software; you can redistribute it
 and/or modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 3 of
 the License, or (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; If not, see <http://www.gnu.org/licenses/>.
 
 EXCEPTION : As a special exception, you may create a larger work
 that contains this FAUST architecture section and distribute
 that work under terms of your choice, so long as this FAUST
 architecture section is not modified.
 ************************************************************************/

#ifndef __MemoryReader__
#define __MemoryReader__

#include <assert.h>
#include <map>

#include "faust/gui/Soundfile.h"

/*
 A 'MemoryReader' object can be used to prepare a set of sound resources in memory, to be used by SoundUI::addSoundfile.
 */

struct MemoryReader : public SoundfileReader {
    
    std::map<std::string, Soundfile*> fSoundMap;
    
    std::string CheckAux(const std::string& path_name_str)
    {
        return (fSoundMap.find(path_name_str) != fSoundMap.end()) ? path_name_str : "";
    }
    
    ~MemoryReader()
    {
        // Delete all remaining soundfiles
        std::map<std::string, Soundfile*>::iterator it;
        for (it = fSoundMap.begin(); it != fSoundMap.end(); ++it) {
            delete (*it).second;
        }
    }
    
    /*
      The Soundfile is removed from this map to be kept in SoundUI one. 
      A more sophisticated scheme using 'std::shared_ptr' could be used instead.
    */
    Soundfile* Read(const std::string& path_name_str, int max_chan)
    {
        assert(fSoundMap.find(path_name_str) != fSoundMap.end());
        Soundfile* res = fSoundMap[path_name_str];
        fSoundMap.erase(path_name_str);
        return res;
    }
    
    // To be used to prepare all soundfiles
    void Add(const std::string& path_name_str, Soundfile* sf)
    {
        fSoundMap[path_name_str] = sf;
    }
    
    static MemoryReader* gReader;
};

/*
// To be used in SoundUI.h
 
#include "faust/gui/MemoryReader.h"
 
MemoryReader* MemoryReader::gReader = new MemoryReader();
 
MemoryReader::gReader->Add("sound1", buildSoundfile(...));
MemoryReader::gReader->Add("sound2", buildSoundfile(...));
MemoryReader::gReader->Add("sound3", buildSoundfile(...));
 
Soundfile* createSoundfile(const std::string& path_name_str, int max_chan)
{
    return MemoryReader::gReader->Read(path_name_str, max_chan);
}
*/

#endif
