
#include <iostream>

#include "StreamerHepRepInstance.h"

using namespace std;
using namespace HEPREP;

StreamerHepRepInstance::StreamerHepRepInstance(HepRepWriter* stream, HepRepInstance* instance, HepRepType* heprepType)
    : StreamerHepRepAttribute(stream), parent(instance), type(heprepType) {

    if (type == NULL) cerr << "HepRepInstance cannot be created without a HepRepType." << endl;
    stream->write(this);
}

StreamerHepRepInstance::StreamerHepRepInstance(HepRepWriter* stream, HepRepInstanceTree* instanceTree, HepRepType* heprepType)
    : StreamerHepRepAttribute(stream), parent(instanceTree), type(heprepType) {

    if (type == NULL) cerr << "HepRepInstance cannot be created without a HepRepType." << endl;
    stream->write(this);
}

StreamerHepRepInstance::~StreamerHepRepInstance() {
}

HepRepInstance* StreamerHepRepInstance::copy(HepRep*, HepRepInstance*, HepRepSelectFilter*) {
    return NULL;
}

HepRepInstance* StreamerHepRepInstance::copy(HepRep*, HepRepInstanceTree*, HepRepSelectFilter*) {
    return NULL;
}

HepRepType* StreamerHepRepInstance::getType() {
    return type;
}

bool StreamerHepRepInstance::addPoint(HepRepPoint*) {
    return true;
}

vector<HepRepPoint*>* StreamerHepRepInstance::getPoints() {
    return NULL;
}

bool StreamerHepRepInstance::addInstance(HepRepInstance*) {
    return true;
}

void StreamerHepRepInstance::removeInstance(HepRepInstance*) {
}

vector<HepRepInstance*>* StreamerHepRepInstance::getInstances() {
    return NULL;
}

HepRepAttValue* StreamerHepRepInstance::getAttValue(string) {
    return NULL;
}
