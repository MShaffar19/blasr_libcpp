#include "HDFScanDataReader.hpp"

using namespace std;

HDFScanDataReader::HDFScanDataReader() {
    //
    // Assume the file is written without a movie name.  This is
    // flipped when a movie name is found.
    //
    useMovieName    = false;
    useRunCode      = false;
    useWhenStarted  = false;
    fileHasScanData = false;
    movieName = "";
    runCode   = "";
    platformId      = NoPlatform;
    initializedAcqParamsGroup = initializedRunInfoGroup = false;
}

int HDFScanDataReader::InitializeAcqParamsAtoms() {
    if (frameRateAtom.Initialize(acqParamsGroup.group, "FrameRate") == 0) { return 0; }
    if (numFramesAtom.Initialize(acqParamsGroup.group, "NumFrames") == 0) { return 0; }
    if (acqParamsGroup.ContainsAttribute("WhenStarted")) {
        if (whenStartedAtom.Initialize(acqParamsGroup.group, "WhenStarted") == 0) { return 0; }
        useWhenStarted = true;
    }
    return 1;
}

//
// This is created on top of a file that is already opened, so
// instead of initializing by opening a file, it is initialized by
// passing the root group that should contain the ScanData group.  
// When shutting down, no file handle needs to be closed.
//
int HDFScanDataReader::Initialize(HDFGroup *pulseDataGroup) {

    //
    // Initiailze groups for reading data.
    //

    initializedAcqParamsGroup = false;
    initializedRunInfoGroup   = false;
    if (pulseDataGroup->ContainsObject("ScanData") == 0 or 
            scanDataGroup.Initialize(pulseDataGroup->group, "ScanData") == 0) {
        return 0;
    }
    fileHasScanData = true;

    if (scanDataGroup.ContainsObject("DyeSet") == 0 or
            dyeSetGroup.Initialize(scanDataGroup.group, "DyeSet") == 0) {
        return 0;
    }

    if (scanDataGroup.ContainsObject("AcqParams") == 0 or
            acqParamsGroup.Initialize(scanDataGroup.group, "AcqParams") == 0) {
        return 0;
    }
    initializedAcqParamsGroup = true;

    if (scanDataGroup.ContainsObject("RunInfo") == 0 or
            runInfoGroup.Initialize(scanDataGroup.group, "RunInfo") == 0) {
        return 0;
    }
    initializedRunInfoGroup = true;
    if (InitializeAcqParamsAtoms() == 0) {
        return 0;
    }

    //
    // Read in the data that will be used later on either per read or
    // when the entire bas/pls file is read.
    //

    if (ReadPlatformId(platformId) == 0) {
        return 0;
    }

    if (runInfoGroup.ContainsAttribute("RunCode") and
            runCodeAtom.Initialize(runInfoGroup, "RunCode")) {
        useRunCode = true;
    }

    //
    // Load baseMap which maps bases (ATGC) to channel orders.
    // This should always be present.
    //
    if (LoadBaseMap(baseMap) == 0)
        return 0;

    //
    // Attempt to load the movie name.  This is not always present.
    //
    LoadMovieName(movieName);

    return 1;
}

string HDFScanDataReader::GetMovieName() {
    LoadMovieName(movieName);
    return movieName;
}

string HDFScanDataReader::GetRunCode() {
    return runCode;
}

int HDFScanDataReader::Read(ScanData &scanData) {
    // All parameters below are required.
    if (ReadPlatformId(scanData.platformId) == 0) return 0;
    LoadMovieName(scanData.movieName);
    LoadBaseMap(scanData.baseMap);

    if (useRunCode) {
        runCodeAtom.Read(scanData.runCode);
    }
    frameRateAtom.Read(scanData.frameRate);
    numFramesAtom.Read(scanData.numFrames);

    if (useWhenStarted) {
        whenStartedAtom.Read(scanData.whenStarted);
    }

}

void HDFScanDataReader::ReadWhenStarted(string &whenStarted) {
    whenStartedAtom.Read(whenStarted);
}

PlatformId HDFScanDataReader::GetPlatformId() {
    return platformId;
}

int HDFScanDataReader::ReadPlatformId(PlatformId &pid) {
    if (runInfoGroup.ContainsAttribute("PlatformId")) {
        if (platformIdAtom.Initialize(runInfoGroup, "PlatformId") == 0) {
            return 0;
        }
        platformIdAtom.Read((unsigned int&)pid);
    }
    else {
        pid = Astro;
    }
    return 1;
}

int HDFScanDataReader::LoadMovieName(string &movieName) {
    // Groups for building read names
    if (runInfoGroup.ContainsAttribute("MovieName") and
            movieNameAtom.Initialize(runInfoGroup, "MovieName")) {
        useMovieName = true;
        movieNameAtom.Read(movieName);
        int e = movieName.size() - 1;
        while (e > 0 and movieName[e] == ' ') e--;
        movieName= movieName.substr(0,e+1);
        return 1;
    }
    else {
        return 0;
    }
}

int HDFScanDataReader::LoadBaseMap(map<char, int> & baseMap) {
    // Map bases to channel order in hdf pls file.
    if (dyeSetGroup.ContainsAttribute("BaseMap") and
            baseMapAtom.Initialize(dyeSetGroup, "BaseMap")) {
        string baseMapStr;
        baseMapAtom.Read(baseMapStr);
        if (baseMapStr.size() != 4) {
            cout << "ERROR, there are more than four types of bases "
                << "according to /ScanData/DyeSet/BaseMap." << endl;
            exit(1);
        }
        baseMap.clear();
        for(int i = 0; i < baseMapStr.size(); i++) {
            baseMap[toupper(baseMapStr[i])] = i;
            baseMap[tolower(baseMapStr[i])] = i;
        }
        return 1;
    }
    return 0;
}

void HDFScanDataReader::Close() {
    if (useMovieName) {
        movieNameAtom.dataspace.close();
    }
    if (useRunCode) {
        runCodeAtom.dataspace.close();
    }
    if (useWhenStarted) {
        whenStartedAtom.dataspace.close();
    }
    baseMapAtom.dataspace.close();
    platformIdAtom.dataspace.close();
    frameRateAtom.dataspace.close();
    numFramesAtom.dataspace.close();

    scanDataGroup.Close();
    dyeSetGroup.Close();
    acqParamsGroup.Close();
    runInfoGroup.Close();
}


std::string HDFScanDataReader::GetMovieName_and_Close(std::string & fileName) {
    HDFFile file;
    file.Open(fileName, H5F_ACC_RDONLY);
    int init = 0;

    fileHasScanData = false;
    if (file.rootGroup.ContainsObject("ScanData") == 0 or 
        scanDataGroup.Initialize(file.rootGroup, "ScanData") == 0) {
        return ""; 
    }
    fileHasScanData = true;

    initializedRunInfoGroup = false;
    if (scanDataGroup.ContainsObject("RunInfo") == 0 or
        runInfoGroup.Initialize(scanDataGroup.group, "RunInfo") == 0) {
        return "";
    }
    initializedRunInfoGroup = true;
    
    string movieName;
    LoadMovieName(movieName);
    Close();
    file.Close();
    return movieName;
}