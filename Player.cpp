#include "stdafx.h"

#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

/**
 *  MidiPlayer is a little class for playing midi files in a C++ console app.
 *  This class demonstrates how you can wrap the windows mciSendString()
 *  function to play media files.
 */
class MidiPlayer
{
    private:
    /**
     * MidiStates establshes names for the running states of a MidiPlayer object.
     */
    enum MidiStates {
        NOFILE = 0,
        LOADED_BIT = 1,
        OPENED_BIT = 2,
        PLAYING_BIT = 4,
        PAUSED_BIT = 8,

        FILELOADED = LOADED_BIT,
        MIDIREADY = LOADED_BIT | OPENED_BIT,
        PLAYING = MIDIREADY | PLAYING_BIT,
        PAUSED =  MIDIREADY | PAUSED_BIT,
        STOPPED = MIDIREADY
    };

    /**
     * MCICommand -- Enumerates the mciCommands array elements.
     */
    enum MCICommand {
        MCIOPEN,
        MCIPLAY,
        MCIFINNISH,
        MCIPAUSE,
        MCIRESUME,
        MCISTOP,
        MCICLOSE
    };

    MidiStates state; //Internal state of the midiplayer.
    wchar_t* fileName;  //Name of file to play
    wchar_t* alias;      //Alias Name

    /**
     * private function to allocate and format a mci command string.
     */
    wchar_t* formatMCICommand(MCICommand cmd)
	{
		/**
		* mciCommands[] is an array of mci command strings that the MidiPlayer
		* class can execute.
		*/
		const wchar_t* mciCommands[] = {
				_T("open \"%s\" alias %s"), //Open a midi file and assign an alias
				_T("play %s"),            //Play a given alias
				_T("play %s wait"),        //Play a given alias (don't return until finished -- not very helpful but there it is).
				_T("pause %s"),            //Pause a given alias (seems to pause all midi)
				_T("resume %s"),        //Resume a paused alias (seems to start all midi paused)
				_T("stop %s"),            //Stop a given alias (does NOT stop all)
				_T("close %s")            //Close the file associated with this alias
		};

        wchar_t* ptr = NULL;
        int len = 0;
        switch(cmd) {
            case MCIOPEN: //Special case because it requires 2 arguments
                len = wcslen(fileName) + wcslen(alias) + wcslen(mciCommands[MCIOPEN]);
                ptr = new wchar_t[len];
                wsprintf(ptr, mciCommands[MCIOPEN], fileName, alias);
                break;
            case MCIPLAY:
            case MCIFINNISH:
            case MCIPAUSE:
            case MCIRESUME:
            case MCISTOP:
            case MCICLOSE:
                len = wcslen(alias) + wcslen(mciCommands[cmd]);
                ptr = new wchar_t[len];
                wsprintf(ptr, mciCommands[cmd], alias);
                break;
        };
        return ptr;
    }
   
    bool execCMD(MCICommand cmd) {
        wchar_t *cmdStr = formatMCICommand(cmd);
        int result;
        if (cmdStr) {
            result = mciSendString(cmdStr, NULL, 0, NULL);
            delete[] cmdStr;
        }
        return (result == 0);
    }

    public:
   
    MidiPlayer(const wchar_t * midiFileName, const wchar_t *aliasName) {
        int fileNameLen = wcslen(midiFileName);
        int aliasLen = wcslen(aliasName);
        if (fileNameLen > 0 && aliasLen >0) {
            fileName = new wchar_t[fileNameLen + 1];
            alias = new wchar_t[aliasLen + 1];
            wcscpy(fileName, midiFileName);
            wcscpy(alias, aliasName);
            state = FILELOADED;
        } else {
            fileName = NULL;
            alias = NULL;
            state = NOFILE;
        }
    }
           
    MidiPlayer(const MidiPlayer &that) {
        int fileNameLen = wcslen(that.fileName);
        int aliasLen = wcslen(that.alias);
        if (fileNameLen > 0 && aliasLen >0) {
            fileName = new wchar_t[fileNameLen + 1];
            alias = new wchar_t[aliasLen + 1];
            wcscpy(fileName, that.fileName);
            wcscpy(alias, that.alias);
            state = FILELOADED;
        } else {
            fileName = NULL;
            alias = NULL;
            state = NOFILE;
        }
    }
   
    ~MidiPlayer() {
        this->close();
    }
   
    MidiPlayer& operator=(const MidiPlayer &that) {
        int fileNameLen = wcslen(that.fileName);
        int aliasLen = wcslen(that.alias);
        if (fileNameLen > 0 && aliasLen >0) {
            fileName = new wchar_t[fileNameLen + 1];
            alias = new wchar_t[aliasLen + 1];
            wcscpy(fileName, that.fileName);
            wcscpy(alias, that.alias);
            state = FILELOADED;
        } else {
            fileName = NULL;
            alias = NULL;
            state = NOFILE;
        }
        return *this;    
    }
   
    MidiStates getState() {
        return state;
    }
   
    bool chFile(const wchar_t *midiFileName, const wchar_t* aliasName) {
        bool retVal = false;
        if (state == NOFILE) {
            int fileNameLen = wcslen(midiFileName);
            int aliasLen = wcslen(aliasName);
            if (fileNameLen > 0 && aliasLen >0) {
                fileName = new wchar_t[fileNameLen + 1];
                alias = new wchar_t[aliasLen + 1];
                wcscpy(fileName, midiFileName);
                wcscpy(alias, aliasName);
                state = FILELOADED;
            } else {
                fileName = NULL;
                alias = NULL;
                state = NOFILE;
            }
            retVal = (state == FILELOADED); //Quick convert to bool
        }
        return retVal;    
    }
   
    bool open() {
        if (state == FILELOADED) {
            if (execCMD(MCIOPEN)) { state = MIDIREADY; }
        }
        return (state == MIDIREADY);
    }
   
    bool play() {
        if (state == STOPPED) {
            if (execCMD(MCIPLAY)) { state = PLAYING; }
			
        }
        return (state == PLAYING);
    }

    bool stop() {
        if (state == PLAYING || state == PAUSED) {
            if (execCMD(MCISTOP)) { state = STOPPED; }
        }
        return (state == STOPPED);
    }

    bool pause() {
        if (state == PLAYING) {
            if (execCMD(MCIPAUSE)) { state = PAUSED; }
        }
        return (state == PAUSED);
    }
   
    bool resume() {
        if (state == PAUSED) {
            if (execCMD(MCIRESUME)) { state = PLAYING; }
        }
        return (state == PLAYING);
    }


    bool close() {
        if (state & (FILELOADED > 0)) {
            this->stop();
             execCMD(MCICLOSE);
            state = NOFILE;
        }
        if (fileName != NULL) { delete[] fileName; fileName = NULL; }
        if (alias != NULL) { delete[] alias; alias = NULL; }        
        return (state == NOFILE);
    }

	bool isplaying()
	{
		return (state == PLAYING);
	}

	void setvolume(int vol)
	{
		wchar_t* cmdStr = new wchar_t[256];
		wsprintf(cmdStr, _T("setaudio %s volume to %d"), alias, vol);

		mciSendString(cmdStr, NULL, 0, NULL);
		delete[] cmdStr;
	}
};
