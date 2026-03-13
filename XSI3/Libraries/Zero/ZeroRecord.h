#ifndef ZERORECORD_H
#define ZERORECORD_H


DLL_DECLSPEC int GetConfigInt(char *name, bool record = true);
DLL_DECLSPEC void GetConfigStr(char *name, char *dflt, char *value, int size);

class ZeroJournal {
public:
	// a journal is a recording of all data that is input rather
	// than computed. it is used to playback exact sequences of
	// events for the purpose of debugging. some of the things
	// that need to be included in the journal are command line
	// options, user preference values, input device values,
	// network activity, and clock values

	// store some data in the journal (or play it back)
	static DLL_DECLSPEC void Record(void *data, int size);

	// some shortcuts for common types
	static DLL_DECLSPEC void Record(char &data);
	static DLL_DECLSPEC void Record(short &data);
	static DLL_DECLSPEC void Record(int &data);
	static DLL_DECLSPEC void Record(float &data);

	// see if we need to actually do something rather than just
	// play some recorded data
	static DLL_DECLSPEC bool NotPlaying(void);
};

class ZeroValue {
public:
	// value recording is for tracking down sync bugs and
	// playback bugs. sprinkle value checks thru out the
	// code to verify that computations are progressing the
	// same on all machines.

	enum Type {
		NEVER,		// value is not recorded, only used internally
		BASIC,		// basic network sync'd value, for detecting sync errors
		FULL,		// detailed network sync'd value, for debugging sync errors
		EXTENDED	// non networked sync'd value, for debugging playback
	};

	// record a value for sync tracking, different levels can
	// be turned on to track deeper and deeper into a problem
	static DLL_DECLSPEC void Record(char value, Type level = FULL);
	static DLL_DECLSPEC void Record(short value, Type level = FULL);
	static DLL_DECLSPEC void Record(int value, Type level = FULL);
	static DLL_DECLSPEC void Record(float value, Type level = FULL);

	// get a checksum of values to see if the game is in sync
	static DLL_DECLSPEC int GetCheckSum(void);

	// start the checksum value at a given value
	static DLL_DECLSPEC void SetCheckSum(int seed);

	// set the current stream to record network sync values on
	static DLL_DECLSPEC void SetStream(int stream);

	enum Event {
		CMP_OFF,
		CMP_ON,
		IO_OFF,
		IO_ON,
		BRK_OFF,
		BRK_ON
	};

	static DLL_DECLSPEC void Send(Event e);
};

DLL_DECLSPEC void CloseRecording(void);
DLL_DECLSPEC void ZeroRecord_PreDump(void);
DLL_DECLSPEC void ZeroRecord_PrepLoad(void);
DLL_DECLSPEC void ZeroRecord_PostLoad(void);

#endif
