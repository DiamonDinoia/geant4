//
// ********************************************************************
// * DISCLAIMER                                                       *
// *                                                                  *
// * The following disclaimer summarizes all the specific disclaimers *
// * of contributors to this software. The specific disclaimers,which *
// * govern, are listed with their locations in:                      *
// *   http://cern.ch/geant4/license                                  *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.                                                             *
// *                                                                  *
// * This  code  implementation is the  intellectual property  of the *
// * GEANT4 collaboration.                                            *
// * By copying,  distributing  or modifying the Program (or any work *
// * based  on  the Program)  you indicate  your  acceptance of  this *
// * statement, and all its terms.                                    *
// ********************************************************************
 * 
 * This file is not intended to be easily readable and contains a number of 
 * coding conventions designed to improve portability and efficiency. Do not make
 * changes to this file unless you know what you are doing--modify the SWIG 
 * interface file instead. 
 * ----------------------------------------------------------------------------- */

#define SWIGPYTHON

#ifdef __cplusplus
template<class T> class SwigValueWrapper {
    T *tt;
public:
    inline SwigValueWrapper() : tt(0) { }
    inline ~SwigValueWrapper() { if (tt) delete tt; } 
    inline SwigValueWrapper& operator=(const T& t) { tt = new T(t); return *this; }
    inline operator T&() const { return *tt; }
    inline T *operator&() { return tt; }
};                                                    
#endif


#include "Python.h"

/***********************************************************************
 * common.swg
 *
 *     This file contains generic SWIG runtime support for pointer
 *     type checking as well as a few commonly used macros to control
 *     external linkage.
 *
 * Author : David Beazley (beazley@cs.uchicago.edu)
 *
 * Copyright (c) 1999-2000, The University of Chicago
 * 
 * This file may be freely redistributed without license or fee provided
 * this copyright message remains intact.
 ************************************************************************/

#include <string.h>

#if defined(_WIN32) || defined(__WIN32__)
#       if defined(_MSC_VER)
#               if defined(STATIC_LINKED)
#                       define SWIGEXPORT(a) a
#                       define SWIGIMPORT(a) extern a
#               else
#                       define SWIGEXPORT(a) __declspec(dllexport) a
#                       define SWIGIMPORT(a) extern a
#               endif
#       else
#               if defined(__BORLANDC__)
#                       define SWIGEXPORT(a) a _export
#                       define SWIGIMPORT(a) a _export
#               else
#                       define SWIGEXPORT(a) a
#                       define SWIGIMPORT(a) a
#               endif
#       endif
#else
#       define SWIGEXPORT(a) a
#       define SWIGIMPORT(a) a
#endif

#ifdef SWIG_GLOBAL
#define SWIGRUNTIME(a) SWIGEXPORT(a)
#else
#define SWIGRUNTIME(a) static a
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef void *(*swig_converter_func)(void *);
typedef struct swig_type_info *(*swig_dycast_func)(void **);

typedef struct swig_type_info {
  const char             *name;                 
  swig_converter_func     converter;
  const char             *str;
  void                   *clientdata;	
  swig_dycast_func        dcast;
  struct swig_type_info  *next;
  struct swig_type_info  *prev;
} swig_type_info;

#ifdef SWIG_NOINCLUDE

SWIGIMPORT(swig_type_info *) SWIG_TypeRegister(swig_type_info *);
SWIGIMPORT(swig_type_info *) SWIG_TypeCheck(char *c, swig_type_info *);
SWIGIMPORT(void *)           SWIG_TypeCast(swig_type_info *, void *);
SWIGIMPORT(swig_type_info *) SWIG_TypeDynamicCast(swig_type_info *, void **);
SWIGIMPORT(swig_type_info *) SWIG_TypeQuery(const char *);
SWIGIMPORT(void)             SWIG_TypeClientData(swig_type_info *, void *);

#else

static swig_type_info *swig_type_list = 0;

/* Register a type mapping with the type-checking */
SWIGRUNTIME(swig_type_info *)
SWIG_TypeRegister(swig_type_info *ti)
{
  swig_type_info *tc, *head, *ret, *next;
  /* Check to see if this type has already been registered */
  tc = swig_type_list;
  while (tc) {
    if (strcmp(tc->name, ti->name) == 0) {
      /* Already exists in the table.  Just add additional types to the list */
      if (tc->clientdata) ti->clientdata = tc->clientdata;	
      head = tc;
      next = tc->next;
      goto l1;
    }
    tc = tc->prev;
  }
  head = ti;
  next = 0;

  /* Place in list */
  ti->prev = swig_type_list;
  swig_type_list = ti;

  /* Build linked lists */
 l1:
  ret = head;
  tc = ti + 1;
  /* Patch up the rest of the links */
  while (tc->name) {
    head->next = tc;
    tc->prev = head;
    head = tc;
    tc++;
  }
  head->next = next;
  return ret;
}

/* Check the typename */
SWIGRUNTIME(swig_type_info *) 
SWIG_TypeCheck(char *c, swig_type_info *ty)
{
  swig_type_info *s;
  if (!ty) return 0;        /* Void pointer */
  s = ty->next;             /* First element always just a name */
  do {
    if (strcmp(s->name,c) == 0) {
      if (s == ty->next) return s;
      /* Move s to the top of the linked list */
      s->prev->next = s->next;
      if (s->next) {
	s->next->prev = s->prev;
      }
      /* Insert s as second element in the list */
      s->next = ty->next;
      if (ty->next) ty->next->prev = s;
      ty->next = s;
      return s;
    }
    s = s->next;
  } while (s && (s != ty->next));
  return 0;
}

/* Cast a pointer up an inheritance hierarchy */
SWIGRUNTIME(void *) 
SWIG_TypeCast(swig_type_info *ty, void *ptr) 
{
  if ((!ty) || (!ty->converter)) return ptr;
  return (*ty->converter)(ptr);
}

/* Dynamic pointer casting. Down an inheritance hierarchy */
SWIGRUNTIME(swig_type_info *) 
SWIG_TypeDynamicCast(swig_type_info *ty, void **ptr) 
{
  swig_type_info *lastty = ty;
  if (!ty || !ty->dcast) return ty;
  while (ty && (ty->dcast)) {
     ty = (*ty->dcast)(ptr);
     if (ty) lastty = ty;
  }
  return lastty;
}

/* Search for a swig_type_info structure */
SWIGRUNTIME(swig_type_info *)
SWIG_TypeQuery(const char *name) {
  swig_type_info *ty = swig_type_list;
  while (ty) {
    if (ty->str && (strcmp(name,ty->str) == 0)) return ty;
    if (ty->name && (strcmp(name,ty->name) == 0)) return ty;
    ty = ty->prev;
  }
  return 0;
}

/* Set the clientdata field for a type */
SWIGRUNTIME(void)
SWIG_TypeClientData(swig_type_info *ti, void *clientdata) {
  swig_type_info *tc, *equiv;
  if (ti->clientdata) return;
  ti->clientdata = clientdata;
  equiv = ti->next;
  while (equiv) {
    if (!equiv->converter) {
      tc = swig_type_list;
      while (tc) {
	if ((strcmp(tc->name, equiv->name) == 0))
	  SWIG_TypeClientData(tc,clientdata);
	tc = tc->prev;
      }
    }
    equiv = equiv->next;
  }
}
#endif

#ifdef __cplusplus
}

#endif

/***********************************************************************
 * python.swg
 *
 *     This file contains the runtime support for Python modules
 *     and includes code for managing global variables and pointer
 *     type checking.
 *
 * Author : David Beazley (beazley@cs.uchicago.edu)
 ************************************************************************/

#include "Python.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SWIG_PY_INT     1
#define SWIG_PY_FLOAT   2
#define SWIG_PY_STRING  3
#define SWIG_PY_POINTER 4
#define SWIG_PY_BINARY  5

/* Flags for pointer conversion */

#define SWIG_POINTER_EXCEPTION     0x1
#define SWIG_POINTER_DISOWN        0x2

/* Exception handling in wrappers */
#define SWIG_fail   goto fail

/* Constant information structure */
typedef struct swig_const_info {
    int type;
    char *name;
    long lvalue;
    double dvalue;
    void   *pvalue;
    swig_type_info **ptype;
} swig_const_info;

#ifdef SWIG_NOINCLUDE

SWIGEXPORT(PyObject *)        SWIG_newvarlink();
SWIGEXPORT(void)              SWIG_addvarlink(PyObject *, char *, PyObject *(*)(void), int (*)(PyObject *));
SWIGEXPORT(int)               SWIG_ConvertPtr(PyObject *, void **, swig_type_info *, int);
SWIGEXPORT(int)               SWIG_ConvertPacked(PyObject *, void *, int sz, swig_type_info *, int);
SWIGEXPORT(char *)            SWIG_PackData(char *c, void *, int);
SWIGEXPORT(char *)            SWIG_UnpackData(char *c, void *, int);
SWIGEXPORT(PyObject *)        SWIG_NewPointerObj(void *, swig_type_info *,int own);
SWIGEXPORT(PyObject *)        SWIG_NewPackedObj(void *, int sz, swig_type_info *);
SWIGEXPORT(void)              SWIG_InstallConstants(PyObject *d, swig_const_info constants[]);
#else

/* -----------------------------------------------------------------------------
 * global variable support code.
 * ----------------------------------------------------------------------------- */

typedef struct swig_globalvar {   
  char       *name;                  /* Name of global variable */
  PyObject *(*get_attr)(void);       /* Return the current value */
  int       (*set_attr)(PyObject *); /* Set the value */
  struct swig_globalvar *next;
} swig_globalvar;

typedef struct swig_varlinkobject {
  PyObject_HEAD
  swig_globalvar *vars;
} swig_varlinkobject;

static PyObject *
swig_varlink_repr(swig_varlinkobject *v) {
  v = v;
  return PyString_FromString("<Global variables>");
}

static int
swig_varlink_print(swig_varlinkobject *v, FILE *fp, int flags) {
  swig_globalvar  *var;
  flags = flags;
  fprintf(fp,"Global variables { ");
  for (var = v->vars; var; var=var->next) {
    fprintf(fp,"%s", var->name);
    if (var->next) fprintf(fp,", ");
  }
  fprintf(fp," }\n");
  return 0;
}

static PyObject *
swig_varlink_getattr(swig_varlinkobject *v, char *n) {
  swig_globalvar *var = v->vars;
  while (var) {
    if (strcmp(var->name,n) == 0) {
      return (*var->get_attr)();
    }
    var = var->next;
  }
  PyErr_SetString(PyExc_NameError,"Unknown C global variable");
  return NULL;
}

static int
swig_varlink_setattr(swig_varlinkobject *v, char *n, PyObject *p) {
  swig_globalvar *var = v->vars;
  while (var) {
    if (strcmp(var->name,n) == 0) {
      return (*var->set_attr)(p);
    }
    var = var->next;
  }
  PyErr_SetString(PyExc_NameError,"Unknown C global variable");
  return 1;
}

statichere PyTypeObject varlinktype = {
  PyObject_HEAD_INIT(0)              
  0,
  (char *)"swigvarlink",                      /* Type name    */
  sizeof(swig_varlinkobject),         /* Basic size   */
  0,                                  /* Itemsize     */
  0,                                  /* Deallocator  */ 
  (printfunc) swig_varlink_print,     /* Print        */
  (getattrfunc) swig_varlink_getattr, /* get attr     */
  (setattrfunc) swig_varlink_setattr, /* Set attr     */
  0,                                  /* tp_compare   */
  (reprfunc) swig_varlink_repr,       /* tp_repr      */    
  0,                                  /* tp_as_number */
  0,                                  /* tp_as_mapping*/
  0,                                  /* tp_hash      */
};

/* Create a variable linking object for use later */
SWIGRUNTIME(PyObject *)
SWIG_newvarlink(void) {
  swig_varlinkobject *result = 0;
  result = PyMem_NEW(swig_varlinkobject,1);
  varlinktype.ob_type = &PyType_Type;    /* Patch varlinktype into a PyType */
  result->ob_type = &varlinktype;
  result->vars = 0;
  result->ob_refcnt = 0;
  Py_XINCREF((PyObject *) result);
  return ((PyObject*) result);
}

SWIGRUNTIME(void)
SWIG_addvarlink(PyObject *p, char *name,
	   PyObject *(*get_attr)(void), int (*set_attr)(PyObject *p)) {
  swig_varlinkobject *v;
  swig_globalvar *gv;
  v= (swig_varlinkobject *) p;
  gv = (swig_globalvar *) malloc(sizeof(swig_globalvar));
  gv->name = (char *) malloc(strlen(name)+1);
  strcpy(gv->name,name);
  gv->get_attr = get_attr;
  gv->set_attr = set_attr;
  gv->next = v->vars;
  v->vars = gv;
}

/* Pack binary data into a string */
SWIGRUNTIME(char *)
SWIG_PackData(char *c, void *ptr, int sz) {
  static char hex[17] = "0123456789abcdef";
  int i;
  unsigned char *u = (unsigned char *) ptr;
  register unsigned char uu;
  for (i = 0; i < sz; i++,u++) {
    uu = *u;
    *(c++) = hex[(uu & 0xf0) >> 4];
    *(c++) = hex[uu & 0xf];
  }
  return c;
}

/* Unpack binary data from a string */
SWIGRUNTIME(char *)
SWIG_UnpackData(char *c, void *ptr, int sz) {
  register unsigned char uu = 0;
  register int d;
  unsigned char *u = (unsigned char *) ptr;
  int i;
  for (i = 0; i < sz; i++, u++) {
    d = *(c++);
    if ((d >= '0') && (d <= '9'))
      uu = ((d - '0') << 4);
    else if ((d >= 'a') && (d <= 'f'))
      uu = ((d - ('a'-10)) << 4);
    d = *(c++);
    if ((d >= '0') && (d <= '9'))
      uu |= (d - '0');
    else if ((d >= 'a') && (d <= 'f'))
      uu |= (d - ('a'-10));
    *u = uu;
  }
  return c;
}

/* Convert a pointer value */
SWIGRUNTIME(int)
SWIG_ConvertPtr(PyObject *obj, void **ptr, swig_type_info *ty, int flags) {
  swig_type_info *tc;
  char  *c;
  static PyObject *SWIG_this = 0;
  int    newref = 0;
  PyObject  *pyobj = 0;

  if (!obj) return 0;
  if (obj == Py_None) {
    *ptr = 0;
    return 0;
  }
#ifdef SWIG_COBJECT_TYPES
  if (!(PyCObject_Check(obj))) {
    if (!SWIG_this)
      SWIG_this = PyString_FromString("this");
    pyobj = obj;
    obj = PyObject_GetAttr(obj,SWIG_this);
    newref = 1;
    if (!obj) goto type_error;
    if (!PyCObject_Check(obj)) {
      Py_DECREF(obj);
      goto type_error;
    }
  }  
  *ptr = PyCObject_AsVoidPtr(obj);
  c = (char *) PyCObject_GetDesc(obj);
  if (newref) Py_DECREF(obj);
  goto cobject;
#else
  if (!(PyString_Check(obj))) {
    if (!SWIG_this)
      SWIG_this = PyString_FromString("this");
    pyobj = obj;
    obj = PyObject_GetAttr(obj,SWIG_this);
    newref = 1;
    if (!obj) goto type_error;
    if (!PyString_Check(obj)) {
      Py_DECREF(obj);
      goto type_error;
    }
  } 
  c = PyString_AsString(obj);
  /* Pointer values must start with leading underscore */
  if (*c != '_') {
    *ptr = (void *) 0;
    if (strcmp(c,"NULL") == 0) {
      if (newref) { Py_DECREF(obj); }
      return 0;
    } else {
      if (newref) { Py_DECREF(obj); }
      goto type_error;
    }
  }
  c++;
  c = SWIG_UnpackData(c,ptr,sizeof(void *));
  if (newref) { Py_DECREF(obj); }
#endif

#ifdef SWIG_COBJECT_TYPES
cobject:
#endif

  if (ty) {
    tc = SWIG_TypeCheck(c,ty);
    if (!tc) goto type_error;
    *ptr = SWIG_TypeCast(tc,(void*) *ptr);
  }

  if ((pyobj) && (flags & SWIG_POINTER_DISOWN)) {
      PyObject *zero = PyInt_FromLong(0);
      PyObject_SetAttrString(pyobj,"thisown",zero);
      Py_DECREF(zero);
  }
  return 0;

type_error:
  if (flags & SWIG_POINTER_EXCEPTION) {
    if (ty) {
      char *temp = (char *) malloc(64+strlen(ty->name));
      sprintf(temp,"Type error. Expected %s", ty->name);
      PyErr_SetString(PyExc_TypeError, temp);
      free((char *) temp);
    } else {
      PyErr_SetString(PyExc_TypeError,"Expected a pointer");
    }
  }
  return -1;
}

/* Convert a packed value value */
SWIGRUNTIME(int)
SWIG_ConvertPacked(PyObject *obj, void *ptr, int sz, swig_type_info *ty, int flags) {
  swig_type_info *tc;
  char  *c;

  if ((!obj) || (!PyString_Check(obj))) goto type_error;
  c = PyString_AsString(obj);
  /* Pointer values must start with leading underscore */
  if (*c != '_') goto type_error;
  c++;
  c = SWIG_UnpackData(c,ptr,sz);
  if (ty) {
    tc = SWIG_TypeCheck(c,ty);
    if (!tc) goto type_error;
  }
  return 0;

type_error:

  if (flags) {
    if (ty) {
      char *temp = (char *) malloc(64+strlen(ty->name));
      sprintf(temp,"Type error. Expected %s", ty->name);
      PyErr_SetString(PyExc_TypeError, temp);
      free((char *) temp);
    } else {
      PyErr_SetString(PyExc_TypeError,"Expected a pointer");
    }
  }
  return -1;
}

/* Create a new pointer object */
SWIGRUNTIME(PyObject *)
SWIG_NewPointerObj(void *ptr, swig_type_info *type, int own) {
  PyObject *robj;
  if (!ptr) {
    Py_INCREF(Py_None);
    return Py_None;
  }
#ifdef SWIG_COBJECT_TYPES
  robj = PyCObject_FromVoidPtrAndDesc((void *) ptr, (char *) type->name, NULL);
#else
  {
    char result[1024];
    char *r = result;
    *(r++) = '_';
    r = SWIG_PackData(r,&ptr,sizeof(void *));
    strcpy(r,type->name);
    robj = PyString_FromString(result);
  }
#endif
  if (!robj || (robj == Py_None)) return robj;
  if (type->clientdata) {
    PyObject *inst;
    PyObject *args = Py_BuildValue((char*)"(O)", robj);
    Py_DECREF(robj);
    inst = PyObject_CallObject((PyObject *) type->clientdata, args);
    Py_DECREF(args);
    if (inst) {
      if (own) {
	PyObject *n = PyInt_FromLong(1);
	PyObject_SetAttrString(inst,(char*)"thisown",n);
	Py_DECREF(n);
      }
      robj = inst;
    }
  }
  return robj;
}

SWIGRUNTIME(PyObject *)
SWIG_NewPackedObj(void *ptr, int sz, swig_type_info *type) {
  char result[1024];
  char *r = result;
  if ((2*sz + 1 + strlen(type->name)) > 1000) return 0;
  *(r++) = '_';
  r = SWIG_PackData(r,ptr,sz);
  strcpy(r,type->name);
  return PyString_FromString(result);
}

/* Install Constants */
SWIGRUNTIME(void)
SWIG_InstallConstants(PyObject *d, swig_const_info constants[]) {
  int i;
  PyObject *obj;
  for (i = 0; constants[i].type; i++) {
    switch(constants[i].type) {
    case SWIG_PY_INT:
      obj = PyInt_FromLong(constants[i].lvalue);
      break;
    case SWIG_PY_FLOAT:
      obj = PyFloat_FromDouble(constants[i].dvalue);
      break;
    case SWIG_PY_STRING:
      obj = PyString_FromString((char *) constants[i].pvalue);
      break;
    case SWIG_PY_POINTER:
      obj = SWIG_NewPointerObj(constants[i].pvalue, *(constants[i]).ptype,0);
      break;
    case SWIG_PY_BINARY:
      obj = SWIG_NewPackedObj(constants[i].pvalue, constants[i].lvalue, *(constants[i].ptype));
      break;
    default:
      obj = 0;
      break;
    }
    if (obj) {
      PyDict_SetItemString(d,constants[i].name,obj);
      Py_DECREF(obj);
    }
  }
}

#endif

#ifdef __cplusplus
}
#endif








/* -------- TYPES TABLE (BEGIN) -------- */

#define  SWIGTYPE_p_G4RunManager swig_types[0] 
#define  SWIGTYPE_p_G4ScoreTable swig_types[1] 
#define  SWIGTYPE_p_G4VImportanceAlgorithm swig_types[2] 
#define  SWIGTYPE_p_G4CellScoreValues swig_types[3] 
#define  SWIGTYPE_p_G4IStore swig_types[4] 
#define  SWIGTYPE_p_G4VIStore swig_types[5] 
#define  SWIGTYPE_p_G4ParallelGeometrySampler swig_types[6] 
#define  SWIGTYPE_p_G4CellScorerStore swig_types[7] 
#define  SWIGTYPE_p_G4VCellScorerStore swig_types[8] 
#define  SWIGTYPE_p_B03AppBase swig_types[9] 
#define  SWIGTYPE_p_G4std__ostream swig_types[10] 
#define  SWIGTYPE_p_G4CellStoreScorer swig_types[11] 
#define  SWIGTYPE_p_G4CellScoreComposer swig_types[12] 
#define  SWIGTYPE_p_G4GeometryCell swig_types[13] 
#define  SWIGTYPE_p_G4MapGeometryCellCellScorer swig_types[14] 
#define  SWIGTYPE_p_G4CellScorer swig_types[15] 
#define  SWIGTYPE_p_G4VScorer swig_types[16] 
#define  SWIGTYPE_p_G4bool swig_types[17] 
#define  SWIGTYPE_p_G4VPhysicalVolume swig_types[18] 
#define  SWIGTYPE_p_G4double swig_types[19] 
#define  SWIGTYPE_p_B03ImportanceDetectorConstruction swig_types[20] 
static swig_type_info *swig_types[22];

/* -------- TYPES TABLE (END) -------- */


/*-----------------------------------------------
              @(target):= _B03App.so
  ------------------------------------------------*/
#define SWIG_init    init_B03App

#define SWIG_name    "_B03App"

#include "globals.hh"
#include "B03App.hh"
#include "G4RunManager.hh"
#include "G4IStore.hh"
#include "G4VCellScorerStore.hh"
#include "G4VScorer.hh"
#include "G4ParallelGeometrySampler.hh"
#include "G4CellScorerStore.hh"
#include "G4CellStoreScorer.hh"
#include "G4GeometryCell.hh"
#include "G4CellScoreComposer.hh"
#include "G4CellScoreValues.hh"
#include "G4CellScorer.hh"
#include "G4ScoreTable.hh"
#include "G4VIStore.hh"
#include <string>
#include "g4std/strstream"
#include <memory>
#include "G4VPhysicalVolume.hh"
#include "B03ImportanceDetectorConstruction.hh"
#include "G4VImportanceAlgorithm.hh"

char const *G4ScoreTable_Write(G4ScoreTable *self,G4MapGeometryCellCellScorer const &cs){
      G4std::ostrstream tmpout;
      self->Print(cs, &tmpout);
      string *value = new string(tmpout.str());
      return value->c_str();
    }
char const *B03ImportanceDetectorConstruction_ListPhysNames(B03ImportanceDetectorConstruction *self){
      G4String *value = new G4String(self->ListPhysNamesAsG4String());
      return value->c_str();
    }
#ifdef __cplusplus
extern "C" {
#endif
static PyObject *_wrap_G4RunManager_BeamOn(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    G4RunManager *arg1 ;
    int arg2 ;
    PyObject * obj0  = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"Oi:G4RunManager_BeamOn",&obj0,&arg2)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_G4RunManager,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    (arg1)->BeamOn(arg2);
    
    Py_INCREF(Py_None); resultobj = Py_None;
    return resultobj;
    fail:
    return NULL;
}


static PyObject * G4RunManager_swigregister(PyObject *self, PyObject *args) {
    PyObject *obj;
    if (!PyArg_ParseTuple(args,(char*)"O", &obj)) return NULL;
    SWIG_TypeClientData(SWIGTYPE_p_G4RunManager, obj);
    Py_INCREF(obj);
    return Py_BuildValue((char *)"");
}
static PyObject * G4VIStore_swigregister(PyObject *self, PyObject *args) {
    PyObject *obj;
    if (!PyArg_ParseTuple(args,(char*)"O", &obj)) return NULL;
    SWIG_TypeClientData(SWIGTYPE_p_G4VIStore, obj);
    Py_INCREF(obj);
    return Py_BuildValue((char *)"");
}
static PyObject * G4VPhysicalVolume_swigregister(PyObject *self, PyObject *args) {
    PyObject *obj;
    if (!PyArg_ParseTuple(args,(char*)"O", &obj)) return NULL;
    SWIG_TypeClientData(SWIGTYPE_p_G4VPhysicalVolume, obj);
    Py_INCREF(obj);
    return Py_BuildValue((char *)"");
}
static PyObject *_wrap_new_G4GeometryCell(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    G4VPhysicalVolume *arg1 ;
    int arg2 ;
    G4GeometryCell *result;
    PyObject * obj0  = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"Oi:new_G4GeometryCell",&obj0,&arg2)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_G4VPhysicalVolume,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    result = (G4GeometryCell *)new G4GeometryCell((G4VPhysicalVolume const &)*arg1,arg2);
    
    resultobj = SWIG_NewPointerObj((void *) result, SWIGTYPE_p_G4GeometryCell, 1);
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_delete_G4GeometryCell(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    G4GeometryCell *arg1 ;
    PyObject * obj0  = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"O:delete_G4GeometryCell",&obj0)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_G4GeometryCell,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    delete arg1;
    
    Py_INCREF(Py_None); resultobj = Py_None;
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_G4GeometryCell_GetPhysicalVolume(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    G4GeometryCell *arg1 ;
    G4VPhysicalVolume *result;
    PyObject * obj0  = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"O:G4GeometryCell_GetPhysicalVolume",&obj0)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_G4GeometryCell,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    {
        G4VPhysicalVolume const &_result_ref = ((G4GeometryCell const *)arg1)->GetPhysicalVolume();
        result = (G4VPhysicalVolume *) &_result_ref;
    }
    
    resultobj = SWIG_NewPointerObj((void *) result, SWIGTYPE_p_G4VPhysicalVolume, 0);
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_G4GeometryCell_GetReplicaNumber(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    G4GeometryCell *arg1 ;
    int result;
    PyObject * obj0  = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"O:G4GeometryCell_GetReplicaNumber",&obj0)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_G4GeometryCell,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    result = (int)((G4GeometryCell const *)arg1)->GetReplicaNumber();
    
    resultobj = PyInt_FromLong((long)result);
    return resultobj;
    fail:
    return NULL;
}


static PyObject * G4GeometryCell_swigregister(PyObject *self, PyObject *args) {
    PyObject *obj;
    if (!PyArg_ParseTuple(args,(char*)"O", &obj)) return NULL;
    SWIG_TypeClientData(SWIGTYPE_p_G4GeometryCell, obj);
    Py_INCREF(obj);
    return Py_BuildValue((char *)"");
}
static PyObject *_wrap_new_G4IStore(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    G4VPhysicalVolume *arg1 ;
    G4IStore *result;
    PyObject * obj0  = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"O:new_G4IStore",&obj0)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_G4VPhysicalVolume,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    result = (G4IStore *)new G4IStore((G4VPhysicalVolume const &)*arg1);
    
    resultobj = SWIG_NewPointerObj((void *) result, SWIGTYPE_p_G4IStore, 1);
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_delete_G4IStore(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    G4IStore *arg1 ;
    PyObject * obj0  = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"O:delete_G4IStore",&obj0)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_G4IStore,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    delete arg1;
    
    Py_INCREF(Py_None); resultobj = Py_None;
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_G4IStore_AddImportanceGeometryCell(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    G4IStore *arg1 ;
    double arg2 ;
    G4GeometryCell *arg3 ;
    PyObject * obj0  = 0 ;
    PyObject * obj2  = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"OdO:G4IStore_AddImportanceGeometryCell",&obj0,&arg2,&obj2)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_G4IStore,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    if ((SWIG_ConvertPtr(obj2,(void **) &arg3, SWIGTYPE_p_G4GeometryCell,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    (arg1)->AddImportanceGeometryCell(arg2,(G4GeometryCell const &)*arg3);
    
    Py_INCREF(Py_None); resultobj = Py_None;
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_G4IStore_ChangeImportance(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    G4IStore *arg1 ;
    double arg2 ;
    G4GeometryCell *arg3 ;
    PyObject * obj0  = 0 ;
    PyObject * obj2  = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"OdO:G4IStore_ChangeImportance",&obj0,&arg2,&obj2)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_G4IStore,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    if ((SWIG_ConvertPtr(obj2,(void **) &arg3, SWIGTYPE_p_G4GeometryCell,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    (arg1)->ChangeImportance(arg2,(G4GeometryCell const &)*arg3);
    
    Py_INCREF(Py_None); resultobj = Py_None;
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_G4IStore_GetImportance(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    G4IStore *arg1 ;
    G4GeometryCell *arg2 ;
    double result;
    PyObject * obj0  = 0 ;
    PyObject * obj1  = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"OO:G4IStore_GetImportance",&obj0,&obj1)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_G4IStore,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    if ((SWIG_ConvertPtr(obj1,(void **) &arg2, SWIGTYPE_p_G4GeometryCell,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    result = (double)((G4IStore const *)arg1)->GetImportance((G4GeometryCell const &)*arg2);
    
    resultobj = PyFloat_FromDouble(result);
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_G4IStore_IsKnown(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    G4IStore *arg1 ;
    G4GeometryCell *arg2 ;
    bool result;
    PyObject * obj0  = 0 ;
    PyObject * obj1  = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"OO:G4IStore_IsKnown",&obj0,&obj1)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_G4IStore,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    if ((SWIG_ConvertPtr(obj1,(void **) &arg2, SWIGTYPE_p_G4GeometryCell,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    result = (bool)((G4IStore const *)arg1)->IsKnown((G4GeometryCell const &)*arg2);
    
    resultobj = PyInt_FromLong((long)result);
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_G4IStore_GetWorldVolume(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    G4IStore *arg1 ;
    G4VPhysicalVolume *result;
    PyObject * obj0  = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"O:G4IStore_GetWorldVolume",&obj0)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_G4IStore,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    {
        G4VPhysicalVolume const &_result_ref = ((G4IStore const *)arg1)->GetWorldVolume();
        result = (G4VPhysicalVolume *) &_result_ref;
    }
    
    resultobj = SWIG_NewPointerObj((void *) result, SWIGTYPE_p_G4VPhysicalVolume, 0);
    return resultobj;
    fail:
    return NULL;
}


static PyObject * G4IStore_swigregister(PyObject *self, PyObject *args) {
    PyObject *obj;
    if (!PyArg_ParseTuple(args,(char*)"O", &obj)) return NULL;
    SWIG_TypeClientData(SWIGTYPE_p_G4IStore, obj);
    Py_INCREF(obj);
    return Py_BuildValue((char *)"");
}
static PyObject *_wrap_new_G4CellScoreValues(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    G4CellScoreValues *result;
    
    if(!PyArg_ParseTuple(args,(char *)":new_G4CellScoreValues")) goto fail;
    result = (G4CellScoreValues *)new G4CellScoreValues();
    
    resultobj = SWIG_NewPointerObj((void *) result, SWIGTYPE_p_G4CellScoreValues, 1);
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_G4CellScoreValues_fSumSL_set(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    G4CellScoreValues *arg1 ;
    double arg2 ;
    PyObject * obj0  = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"Od:G4CellScoreValues_fSumSL_set",&obj0,&arg2)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_G4CellScoreValues,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    if (arg1) (arg1)->fSumSL = arg2;
    
    Py_INCREF(Py_None); resultobj = Py_None;
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_G4CellScoreValues_fSumSL_get(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    G4CellScoreValues *arg1 ;
    double result;
    PyObject * obj0  = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"O:G4CellScoreValues_fSumSL_get",&obj0)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_G4CellScoreValues,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    result = (double) ((arg1)->fSumSL);
    
    resultobj = PyFloat_FromDouble(result);
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_G4CellScoreValues_fSumSLW_set(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    G4CellScoreValues *arg1 ;
    double arg2 ;
    PyObject * obj0  = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"Od:G4CellScoreValues_fSumSLW_set",&obj0,&arg2)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_G4CellScoreValues,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    if (arg1) (arg1)->fSumSLW = arg2;
    
    Py_INCREF(Py_None); resultobj = Py_None;
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_G4CellScoreValues_fSumSLW_get(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    G4CellScoreValues *arg1 ;
    double result;
    PyObject * obj0  = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"O:G4CellScoreValues_fSumSLW_get",&obj0)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_G4CellScoreValues,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    result = (double) ((arg1)->fSumSLW);
    
    resultobj = PyFloat_FromDouble(result);
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_G4CellScoreValues_fSumSLW_v_set(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    G4CellScoreValues *arg1 ;
    double arg2 ;
    PyObject * obj0  = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"Od:G4CellScoreValues_fSumSLW_v_set",&obj0,&arg2)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_G4CellScoreValues,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    if (arg1) (arg1)->fSumSLW_v = arg2;
    
    Py_INCREF(Py_None); resultobj = Py_None;
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_G4CellScoreValues_fSumSLW_v_get(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    G4CellScoreValues *arg1 ;
    double result;
    PyObject * obj0  = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"O:G4CellScoreValues_fSumSLW_v_get",&obj0)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_G4CellScoreValues,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    result = (double) ((arg1)->fSumSLW_v);
    
    resultobj = PyFloat_FromDouble(result);
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_G4CellScoreValues_fSumSLWE_set(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    G4CellScoreValues *arg1 ;
    double arg2 ;
    PyObject * obj0  = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"Od:G4CellScoreValues_fSumSLWE_set",&obj0,&arg2)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_G4CellScoreValues,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    if (arg1) (arg1)->fSumSLWE = arg2;
    
    Py_INCREF(Py_None); resultobj = Py_None;
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_G4CellScoreValues_fSumSLWE_get(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    G4CellScoreValues *arg1 ;
    double result;
    PyObject * obj0  = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"O:G4CellScoreValues_fSumSLWE_get",&obj0)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_G4CellScoreValues,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    result = (double) ((arg1)->fSumSLWE);
    
    resultobj = PyFloat_FromDouble(result);
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_G4CellScoreValues_fSumSLWE_v_set(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    G4CellScoreValues *arg1 ;
    double arg2 ;
    PyObject * obj0  = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"Od:G4CellScoreValues_fSumSLWE_v_set",&obj0,&arg2)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_G4CellScoreValues,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    if (arg1) (arg1)->fSumSLWE_v = arg2;
    
    Py_INCREF(Py_None); resultobj = Py_None;
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_G4CellScoreValues_fSumSLWE_v_get(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    G4CellScoreValues *arg1 ;
    double result;
    PyObject * obj0  = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"O:G4CellScoreValues_fSumSLWE_v_get",&obj0)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_G4CellScoreValues,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    result = (double) ((arg1)->fSumSLWE_v);
    
    resultobj = PyFloat_FromDouble(result);
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_G4CellScoreValues_fSumTracksEntering_set(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    G4CellScoreValues *arg1 ;
    int arg2 ;
    PyObject * obj0  = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"Oi:G4CellScoreValues_fSumTracksEntering_set",&obj0,&arg2)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_G4CellScoreValues,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    if (arg1) (arg1)->fSumTracksEntering = arg2;
    
    Py_INCREF(Py_None); resultobj = Py_None;
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_G4CellScoreValues_fSumTracksEntering_get(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    G4CellScoreValues *arg1 ;
    int result;
    PyObject * obj0  = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"O:G4CellScoreValues_fSumTracksEntering_get",&obj0)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_G4CellScoreValues,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    result = (int) ((arg1)->fSumTracksEntering);
    
    resultobj = PyInt_FromLong((long)result);
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_G4CellScoreValues_fSumPopulation_set(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    G4CellScoreValues *arg1 ;
    int arg2 ;
    PyObject * obj0  = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"Oi:G4CellScoreValues_fSumPopulation_set",&obj0,&arg2)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_G4CellScoreValues,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    if (arg1) (arg1)->fSumPopulation = arg2;
    
    Py_INCREF(Py_None); resultobj = Py_None;
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_G4CellScoreValues_fSumPopulation_get(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    G4CellScoreValues *arg1 ;
    int result;
    PyObject * obj0  = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"O:G4CellScoreValues_fSumPopulation_get",&obj0)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_G4CellScoreValues,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    result = (int) ((arg1)->fSumPopulation);
    
    resultobj = PyInt_FromLong((long)result);
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_G4CellScoreValues_fSumCollisions_set(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    G4CellScoreValues *arg1 ;
    int arg2 ;
    PyObject * obj0  = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"Oi:G4CellScoreValues_fSumCollisions_set",&obj0,&arg2)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_G4CellScoreValues,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    if (arg1) (arg1)->fSumCollisions = arg2;
    
    Py_INCREF(Py_None); resultobj = Py_None;
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_G4CellScoreValues_fSumCollisions_get(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    G4CellScoreValues *arg1 ;
    int result;
    PyObject * obj0  = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"O:G4CellScoreValues_fSumCollisions_get",&obj0)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_G4CellScoreValues,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    result = (int) ((arg1)->fSumCollisions);
    
    resultobj = PyInt_FromLong((long)result);
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_G4CellScoreValues_fSumCollisionsWeight_set(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    G4CellScoreValues *arg1 ;
    double arg2 ;
    PyObject * obj0  = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"Od:G4CellScoreValues_fSumCollisionsWeight_set",&obj0,&arg2)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_G4CellScoreValues,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    if (arg1) (arg1)->fSumCollisionsWeight = arg2;
    
    Py_INCREF(Py_None); resultobj = Py_None;
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_G4CellScoreValues_fSumCollisionsWeight_get(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    G4CellScoreValues *arg1 ;
    double result;
    PyObject * obj0  = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"O:G4CellScoreValues_fSumCollisionsWeight_get",&obj0)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_G4CellScoreValues,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    result = (double) ((arg1)->fSumCollisionsWeight);
    
    resultobj = PyFloat_FromDouble(result);
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_G4CellScoreValues_fNumberWeightedEnergy_set(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    G4CellScoreValues *arg1 ;
    double arg2 ;
    PyObject * obj0  = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"Od:G4CellScoreValues_fNumberWeightedEnergy_set",&obj0,&arg2)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_G4CellScoreValues,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    if (arg1) (arg1)->fNumberWeightedEnergy = arg2;
    
    Py_INCREF(Py_None); resultobj = Py_None;
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_G4CellScoreValues_fNumberWeightedEnergy_get(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    G4CellScoreValues *arg1 ;
    double result;
    PyObject * obj0  = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"O:G4CellScoreValues_fNumberWeightedEnergy_get",&obj0)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_G4CellScoreValues,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    result = (double) ((arg1)->fNumberWeightedEnergy);
    
    resultobj = PyFloat_FromDouble(result);
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_G4CellScoreValues_fFluxWeightedEnergy_set(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    G4CellScoreValues *arg1 ;
    double arg2 ;
    PyObject * obj0  = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"Od:G4CellScoreValues_fFluxWeightedEnergy_set",&obj0,&arg2)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_G4CellScoreValues,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    if (arg1) (arg1)->fFluxWeightedEnergy = arg2;
    
    Py_INCREF(Py_None); resultobj = Py_None;
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_G4CellScoreValues_fFluxWeightedEnergy_get(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    G4CellScoreValues *arg1 ;
    double result;
    PyObject * obj0  = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"O:G4CellScoreValues_fFluxWeightedEnergy_get",&obj0)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_G4CellScoreValues,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    result = (double) ((arg1)->fFluxWeightedEnergy);
    
    resultobj = PyFloat_FromDouble(result);
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_G4CellScoreValues_fAverageTrackWeight_set(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    G4CellScoreValues *arg1 ;
    double arg2 ;
    PyObject * obj0  = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"Od:G4CellScoreValues_fAverageTrackWeight_set",&obj0,&arg2)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_G4CellScoreValues,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    if (arg1) (arg1)->fAverageTrackWeight = arg2;
    
    Py_INCREF(Py_None); resultobj = Py_None;
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_G4CellScoreValues_fAverageTrackWeight_get(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    G4CellScoreValues *arg1 ;
    double result;
    PyObject * obj0  = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"O:G4CellScoreValues_fAverageTrackWeight_get",&obj0)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_G4CellScoreValues,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    result = (double) ((arg1)->fAverageTrackWeight);
    
    resultobj = PyFloat_FromDouble(result);
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_G4CellScoreValues_fImportance_set(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    G4CellScoreValues *arg1 ;
    double arg2 ;
    PyObject * obj0  = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"Od:G4CellScoreValues_fImportance_set",&obj0,&arg2)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_G4CellScoreValues,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    if (arg1) (arg1)->fImportance = arg2;
    
    Py_INCREF(Py_None); resultobj = Py_None;
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_G4CellScoreValues_fImportance_get(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    G4CellScoreValues *arg1 ;
    double result;
    PyObject * obj0  = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"O:G4CellScoreValues_fImportance_get",&obj0)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_G4CellScoreValues,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    result = (double) ((arg1)->fImportance);
    
    resultobj = PyFloat_FromDouble(result);
    return resultobj;
    fail:
    return NULL;
}


static PyObject * G4CellScoreValues_swigregister(PyObject *self, PyObject *args) {
    PyObject *obj;
    if (!PyArg_ParseTuple(args,(char*)"O", &obj)) return NULL;
    SWIG_TypeClientData(SWIGTYPE_p_G4CellScoreValues, obj);
    Py_INCREF(obj);
    return Py_BuildValue((char *)"");
}
static PyObject *_wrap_new_G4CellScoreComposer(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    G4CellScoreComposer *result;
    
    if(!PyArg_ParseTuple(args,(char *)":new_G4CellScoreComposer")) goto fail;
    result = (G4CellScoreComposer *)new G4CellScoreComposer();
    
    resultobj = SWIG_NewPointerObj((void *) result, SWIGTYPE_p_G4CellScoreComposer, 1);
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_delete_G4CellScoreComposer(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    G4CellScoreComposer *arg1 ;
    PyObject * obj0  = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"O:delete_G4CellScoreComposer",&obj0)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_G4CellScoreComposer,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    delete arg1;
    
    Py_INCREF(Py_None); resultobj = Py_None;
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_G4CellScoreComposer_GetStandardCellScoreValues(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    G4CellScoreComposer *arg1 ;
    G4CellScoreValues result;
    PyObject * obj0  = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"O:G4CellScoreComposer_GetStandardCellScoreValues",&obj0)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_G4CellScoreComposer,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    result = ((G4CellScoreComposer const *)arg1)->GetStandardCellScoreValues();
    
    {
        G4CellScoreValues * resultptr;
        resultptr = new G4CellScoreValues((G4CellScoreValues &) result);
        resultobj = SWIG_NewPointerObj((void *) resultptr, SWIGTYPE_p_G4CellScoreValues, 1);
    }
    return resultobj;
    fail:
    return NULL;
}


static PyObject * G4CellScoreComposer_swigregister(PyObject *self, PyObject *args) {
    PyObject *obj;
    if (!PyArg_ParseTuple(args,(char*)"O", &obj)) return NULL;
    SWIG_TypeClientData(SWIGTYPE_p_G4CellScoreComposer, obj);
    Py_INCREF(obj);
    return Py_BuildValue((char *)"");
}
static PyObject *_wrap_G4CellScorer_GetCellScoreComposer(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    G4CellScorer *arg1 ;
    G4CellScoreComposer result;
    PyObject * obj0  = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"O:G4CellScorer_GetCellScoreComposer",&obj0)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_G4CellScorer,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    result = ((G4CellScorer const *)arg1)->GetCellScoreComposer();
    
    {
        G4CellScoreComposer * resultptr;
        resultptr = new G4CellScoreComposer((G4CellScoreComposer &) result);
        resultobj = SWIG_NewPointerObj((void *) resultptr, SWIGTYPE_p_G4CellScoreComposer, 1);
    }
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_G4CellScorer_GetCellScoreValues(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    G4CellScorer *arg1 ;
    G4CellScoreValues result;
    PyObject * obj0  = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"O:G4CellScorer_GetCellScoreValues",&obj0)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_G4CellScorer,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    result = ((G4CellScorer const *)arg1)->GetCellScoreValues();
    
    {
        G4CellScoreValues * resultptr;
        resultptr = new G4CellScoreValues((G4CellScoreValues &) result);
        resultobj = SWIG_NewPointerObj((void *) resultptr, SWIGTYPE_p_G4CellScoreValues, 1);
    }
    return resultobj;
    fail:
    return NULL;
}


static PyObject * G4CellScorer_swigregister(PyObject *self, PyObject *args) {
    PyObject *obj;
    if (!PyArg_ParseTuple(args,(char*)"O", &obj)) return NULL;
    SWIG_TypeClientData(SWIGTYPE_p_G4CellScorer, obj);
    Py_INCREF(obj);
    return Py_BuildValue((char *)"");
}
static PyObject * G4VCellScorerStore_swigregister(PyObject *self, PyObject *args) {
    PyObject *obj;
    if (!PyArg_ParseTuple(args,(char*)"O", &obj)) return NULL;
    SWIG_TypeClientData(SWIGTYPE_p_G4VCellScorerStore, obj);
    Py_INCREF(obj);
    return Py_BuildValue((char *)"");
}
static PyObject *_wrap_new_G4CellScorerStore(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    G4CellScorerStore *result;
    
    if(!PyArg_ParseTuple(args,(char *)":new_G4CellScorerStore")) goto fail;
    result = (G4CellScorerStore *)new G4CellScorerStore();
    
    resultobj = SWIG_NewPointerObj((void *) result, SWIGTYPE_p_G4CellScorerStore, 1);
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_delete_G4CellScorerStore(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    G4CellScorerStore *arg1 ;
    PyObject * obj0  = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"O:delete_G4CellScorerStore",&obj0)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_G4CellScorerStore,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    delete arg1;
    
    Py_INCREF(Py_None); resultobj = Py_None;
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_G4CellScorerStore_SetAutoScorerCreate(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    G4CellScorerStore *arg1 ;
    PyObject * obj0  = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"O:G4CellScorerStore_SetAutoScorerCreate",&obj0)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_G4CellScorerStore,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    (arg1)->SetAutoScorerCreate();
    
    Py_INCREF(Py_None); resultobj = Py_None;
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_G4CellScorerStore_AddCellScorer(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    G4CellScorerStore *arg1 ;
    G4GeometryCell *arg2 ;
    G4CellScorer *result;
    PyObject * obj0  = 0 ;
    PyObject * obj1  = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"OO:G4CellScorerStore_AddCellScorer",&obj0,&obj1)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_G4CellScorerStore,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    if ((SWIG_ConvertPtr(obj1,(void **) &arg2, SWIGTYPE_p_G4GeometryCell,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    result = (G4CellScorer *)(arg1)->AddCellScorer((G4GeometryCell const &)*arg2);
    
    resultobj = SWIG_NewPointerObj((void *) result, SWIGTYPE_p_G4CellScorer, 0);
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_G4CellScorerStore_GetMapGeometryCellCellScorer(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    G4CellScorerStore *arg1 ;
    G4MapGeometryCellCellScorer *result;
    PyObject * obj0  = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"O:G4CellScorerStore_GetMapGeometryCellCellScorer",&obj0)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_G4CellScorerStore,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    {
        G4MapGeometryCellCellScorer const &_result_ref = ((G4CellScorerStore const *)arg1)->GetMapGeometryCellCellScorer();
        result = (G4MapGeometryCellCellScorer *) &_result_ref;
    }
    
    resultobj = SWIG_NewPointerObj((void *) result, SWIGTYPE_p_G4MapGeometryCellCellScorer, 0);
    return resultobj;
    fail:
    return NULL;
}


static PyObject * G4CellScorerStore_swigregister(PyObject *self, PyObject *args) {
    PyObject *obj;
    if (!PyArg_ParseTuple(args,(char*)"O", &obj)) return NULL;
    SWIG_TypeClientData(SWIGTYPE_p_G4CellScorerStore, obj);
    Py_INCREF(obj);
    return Py_BuildValue((char *)"");
}
static PyObject * G4VScorer_swigregister(PyObject *self, PyObject *args) {
    PyObject *obj;
    if (!PyArg_ParseTuple(args,(char*)"O", &obj)) return NULL;
    SWIG_TypeClientData(SWIGTYPE_p_G4VScorer, obj);
    Py_INCREF(obj);
    return Py_BuildValue((char *)"");
}
static PyObject *_wrap_new_G4CellStoreScorer(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    G4VCellScorerStore *arg1 ;
    G4CellStoreScorer *result;
    PyObject * obj0  = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"O:new_G4CellStoreScorer",&obj0)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_G4VCellScorerStore,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    result = (G4CellStoreScorer *)new G4CellStoreScorer(*arg1);
    
    resultobj = SWIG_NewPointerObj((void *) result, SWIGTYPE_p_G4CellStoreScorer, 1);
    return resultobj;
    fail:
    return NULL;
}


static PyObject * G4CellStoreScorer_swigregister(PyObject *self, PyObject *args) {
    PyObject *obj;
    if (!PyArg_ParseTuple(args,(char*)"O", &obj)) return NULL;
    SWIG_TypeClientData(SWIGTYPE_p_G4CellStoreScorer, obj);
    Py_INCREF(obj);
    return Py_BuildValue((char *)"");
}
static PyObject * G4VImportanceAlgorithm_swigregister(PyObject *self, PyObject *args) {
    PyObject *obj;
    if (!PyArg_ParseTuple(args,(char*)"O", &obj)) return NULL;
    SWIG_TypeClientData(SWIGTYPE_p_G4VImportanceAlgorithm, obj);
    Py_INCREF(obj);
    return Py_BuildValue((char *)"");
}
static PyObject *_wrap_new_G4ParallelGeometrySampler(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    G4VPhysicalVolume *arg1 ;
    char *arg2 ;
    G4ParallelGeometrySampler *result;
    PyObject * obj0  = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"Os:new_G4ParallelGeometrySampler",&obj0,&arg2)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_G4VPhysicalVolume,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    result = (G4ParallelGeometrySampler *)new G4ParallelGeometrySampler(*arg1,(char const *)arg2);
    
    resultobj = SWIG_NewPointerObj((void *) result, SWIGTYPE_p_G4ParallelGeometrySampler, 1);
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_delete_G4ParallelGeometrySampler(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    G4ParallelGeometrySampler *arg1 ;
    PyObject * obj0  = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"O:delete_G4ParallelGeometrySampler",&obj0)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_G4ParallelGeometrySampler,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    delete arg1;
    
    Py_INCREF(Py_None); resultobj = Py_None;
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_G4ParallelGeometrySampler_PrepareScoring(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    G4ParallelGeometrySampler *arg1 ;
    G4VScorer *arg2 ;
    PyObject * obj0  = 0 ;
    PyObject * obj1  = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"OO:G4ParallelGeometrySampler_PrepareScoring",&obj0,&obj1)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_G4ParallelGeometrySampler,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    if ((SWIG_ConvertPtr(obj1,(void **) &arg2, SWIGTYPE_p_G4VScorer,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    (arg1)->PrepareScoring(arg2);
    
    Py_INCREF(Py_None); resultobj = Py_None;
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_G4ParallelGeometrySampler_PrepareImportanceSampling(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    G4ParallelGeometrySampler *arg1 ;
    G4VIStore *arg2 ;
    G4VImportanceAlgorithm *arg3 = 0 ;
    PyObject * obj0  = 0 ;
    PyObject * obj1  = 0 ;
    PyObject * obj2  = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"OO|O:G4ParallelGeometrySampler_PrepareImportanceSampling",&obj0,&obj1,&obj2)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_G4ParallelGeometrySampler,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    if ((SWIG_ConvertPtr(obj1,(void **) &arg2, SWIGTYPE_p_G4VIStore,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    if (obj2) {
        if ((SWIG_ConvertPtr(obj2,(void **) &arg3, SWIGTYPE_p_G4VImportanceAlgorithm,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    }
    (arg1)->PrepareImportanceSampling(arg2,(G4VImportanceAlgorithm const *)arg3);
    
    Py_INCREF(Py_None); resultobj = Py_None;
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_G4ParallelGeometrySampler_PrepareWeightRoulett(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    G4ParallelGeometrySampler *arg1 ;
    G4double arg2 ;
    G4double arg3 ;
    G4double arg4 ;
    G4double *argp2 ;
    G4double *argp3 ;
    G4double *argp4 ;
    PyObject * obj0  = 0 ;
    PyObject * obj1  = 0 ;
    PyObject * obj2  = 0 ;
    PyObject * obj3  = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"OOOO:G4ParallelGeometrySampler_PrepareWeightRoulett",&obj0,&obj1,&obj2,&obj3)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_G4ParallelGeometrySampler,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    if ((SWIG_ConvertPtr(obj1,(void **) &argp2, SWIGTYPE_p_G4double,SWIG_POINTER_EXCEPTION) == -1)) SWIG_fail;
    arg2 = *argp2; 
    if ((SWIG_ConvertPtr(obj2,(void **) &argp3, SWIGTYPE_p_G4double,SWIG_POINTER_EXCEPTION) == -1)) SWIG_fail;
    arg3 = *argp3; 
    if ((SWIG_ConvertPtr(obj3,(void **) &argp4, SWIGTYPE_p_G4double,SWIG_POINTER_EXCEPTION) == -1)) SWIG_fail;
    arg4 = *argp4; 
    (arg1)->PrepareWeightRoulett(arg2,arg3,arg4);
    
    Py_INCREF(Py_None); resultobj = Py_None;
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_G4ParallelGeometrySampler_Configure(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    G4ParallelGeometrySampler *arg1 ;
    PyObject * obj0  = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"O:G4ParallelGeometrySampler_Configure",&obj0)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_G4ParallelGeometrySampler,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    (arg1)->Configure();
    
    Py_INCREF(Py_None); resultobj = Py_None;
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_G4ParallelGeometrySampler_ClearSampling(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    G4ParallelGeometrySampler *arg1 ;
    PyObject * obj0  = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"O:G4ParallelGeometrySampler_ClearSampling",&obj0)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_G4ParallelGeometrySampler,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    (arg1)->ClearSampling();
    
    Py_INCREF(Py_None); resultobj = Py_None;
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_G4ParallelGeometrySampler_IsConfigured(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    G4ParallelGeometrySampler *arg1 ;
    G4bool result;
    PyObject * obj0  = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"O:G4ParallelGeometrySampler_IsConfigured",&obj0)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_G4ParallelGeometrySampler,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    result = ((G4ParallelGeometrySampler const *)arg1)->IsConfigured();
    
    {
        G4bool * resultptr;
        resultptr = new G4bool((G4bool &) result);
        resultobj = SWIG_NewPointerObj((void *) resultptr, SWIGTYPE_p_G4bool, 1);
    }
    return resultobj;
    fail:
    return NULL;
}


static PyObject * G4ParallelGeometrySampler_swigregister(PyObject *self, PyObject *args) {
    PyObject *obj;
    if (!PyArg_ParseTuple(args,(char*)"O", &obj)) return NULL;
    SWIG_TypeClientData(SWIGTYPE_p_G4ParallelGeometrySampler, obj);
    Py_INCREF(obj);
    return Py_BuildValue((char *)"");
}
static PyObject *_wrap_new_G4ScoreTable(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    G4VIStore *arg1 = 0 ;
    G4ScoreTable *result;
    PyObject * obj0  = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"|O:new_G4ScoreTable",&obj0)) goto fail;
    if (obj0) {
        if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_G4VIStore,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    }
    result = (G4ScoreTable *)new G4ScoreTable((G4VIStore const *)arg1);
    
    resultobj = SWIG_NewPointerObj((void *) result, SWIGTYPE_p_G4ScoreTable, 1);
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_delete_G4ScoreTable(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    G4ScoreTable *arg1 ;
    PyObject * obj0  = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"O:delete_G4ScoreTable",&obj0)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_G4ScoreTable,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    delete arg1;
    
    Py_INCREF(Py_None); resultobj = Py_None;
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_G4ScoreTable_Print(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    G4ScoreTable *arg1 ;
    G4MapGeometryCellCellScorer *arg2 ;
    G4std::ostream *arg3 = 0 ;
    PyObject * obj0  = 0 ;
    PyObject * obj1  = 0 ;
    PyObject * obj2  = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"OO|O:G4ScoreTable_Print",&obj0,&obj1,&obj2)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_G4ScoreTable,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    if ((SWIG_ConvertPtr(obj1,(void **) &arg2, SWIGTYPE_p_G4MapGeometryCellCellScorer,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    if (obj2) {
        if ((SWIG_ConvertPtr(obj2,(void **) &arg3, SWIGTYPE_p_G4std__ostream,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    }
    (arg1)->Print((G4MapGeometryCellCellScorer const &)*arg2,arg3);
    
    Py_INCREF(Py_None); resultobj = Py_None;
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_G4ScoreTable_Write(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    G4ScoreTable *arg1 ;
    G4MapGeometryCellCellScorer *arg2 ;
    char *result;
    PyObject * obj0  = 0 ;
    PyObject * obj1  = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"OO:G4ScoreTable_Write",&obj0,&obj1)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_G4ScoreTable,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    if ((SWIG_ConvertPtr(obj1,(void **) &arg2, SWIGTYPE_p_G4MapGeometryCellCellScorer,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    result = (char *)G4ScoreTable_Write(arg1,(G4MapGeometryCellCellScorer const &)*arg2);
    
    resultobj = result ? PyString_FromString(result) : Py_BuildValue((char*)"");
    return resultobj;
    fail:
    return NULL;
}


static PyObject * G4ScoreTable_swigregister(PyObject *self, PyObject *args) {
    PyObject *obj;
    if (!PyArg_ParseTuple(args,(char*)"O", &obj)) return NULL;
    SWIG_TypeClientData(SWIGTYPE_p_G4ScoreTable, obj);
    Py_INCREF(obj);
    return Py_BuildValue((char *)"");
}
static PyObject *_wrap_new_B03ImportanceDetectorConstruction(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    B03ImportanceDetectorConstruction *result;
    
    if(!PyArg_ParseTuple(args,(char *)":new_B03ImportanceDetectorConstruction")) goto fail;
    result = (B03ImportanceDetectorConstruction *)new B03ImportanceDetectorConstruction();
    
    resultobj = SWIG_NewPointerObj((void *) result, SWIGTYPE_p_B03ImportanceDetectorConstruction, 1);
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_delete_B03ImportanceDetectorConstruction(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    B03ImportanceDetectorConstruction *arg1 ;
    PyObject * obj0  = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"O:delete_B03ImportanceDetectorConstruction",&obj0)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_B03ImportanceDetectorConstruction,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    delete arg1;
    
    Py_INCREF(Py_None); resultobj = Py_None;
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_B03ImportanceDetectorConstruction_GetPhysicalVolumeByName(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    B03ImportanceDetectorConstruction *arg1 ;
    char *arg2 ;
    G4VPhysicalVolume *result;
    PyObject * obj0  = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"Os:B03ImportanceDetectorConstruction_GetPhysicalVolumeByName",&obj0,&arg2)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_B03ImportanceDetectorConstruction,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    {
        G4VPhysicalVolume const &_result_ref = ((B03ImportanceDetectorConstruction const *)arg1)->GetPhysicalVolumeByName((char const *)arg2);
        result = (G4VPhysicalVolume *) &_result_ref;
    }
    
    resultobj = SWIG_NewPointerObj((void *) result, SWIGTYPE_p_G4VPhysicalVolume, 0);
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_B03ImportanceDetectorConstruction_GetWorldVolume(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    B03ImportanceDetectorConstruction *arg1 ;
    G4VPhysicalVolume *result;
    PyObject * obj0  = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"O:B03ImportanceDetectorConstruction_GetWorldVolume",&obj0)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_B03ImportanceDetectorConstruction,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    {
        G4VPhysicalVolume &_result_ref = ((B03ImportanceDetectorConstruction const *)arg1)->GetWorldVolume();
        result = (G4VPhysicalVolume *) &_result_ref;
    }
    
    resultobj = SWIG_NewPointerObj((void *) result, SWIGTYPE_p_G4VPhysicalVolume, 0);
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_B03ImportanceDetectorConstruction_ListPhysNames(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    B03ImportanceDetectorConstruction *arg1 ;
    char *result;
    PyObject * obj0  = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"O:B03ImportanceDetectorConstruction_ListPhysNames",&obj0)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_B03ImportanceDetectorConstruction,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    result = (char *)B03ImportanceDetectorConstruction_ListPhysNames(arg1);
    
    resultobj = result ? PyString_FromString(result) : Py_BuildValue((char*)"");
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_B03ImportanceDetectorConstruction_GetGeometryCell(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    B03ImportanceDetectorConstruction *arg1 ;
    int arg2 ;
    SwigValueWrapper< G4GeometryCell > result;
    PyObject * obj0  = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"Oi:B03ImportanceDetectorConstruction_GetGeometryCell",&obj0,&arg2)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_B03ImportanceDetectorConstruction,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    result = (arg1)->GetGeometryCell(arg2);
    
    {
        G4GeometryCell * resultptr;
        resultptr = new G4GeometryCell((G4GeometryCell &) result);
        resultobj = SWIG_NewPointerObj((void *) resultptr, SWIGTYPE_p_G4GeometryCell, 1);
    }
    return resultobj;
    fail:
    return NULL;
}


static PyObject * B03ImportanceDetectorConstruction_swigregister(PyObject *self, PyObject *args) {
    PyObject *obj;
    if (!PyArg_ParseTuple(args,(char*)"O", &obj)) return NULL;
    SWIG_TypeClientData(SWIGTYPE_p_B03ImportanceDetectorConstruction, obj);
    Py_INCREF(obj);
    return Py_BuildValue((char *)"");
}
static PyObject *_wrap_delete_B03AppBase(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    B03AppBase *arg1 ;
    PyObject * obj0  = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"O:delete_B03AppBase",&obj0)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_B03AppBase,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    delete arg1;
    
    Py_INCREF(Py_None); resultobj = Py_None;
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_B03AppBase_GetB03AppBase(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    B03AppBase *result;
    
    if(!PyArg_ParseTuple(args,(char *)":B03AppBase_GetB03AppBase")) goto fail;
    {
        B03AppBase &_result_ref = B03AppBase::GetB03AppBase();
        result = (B03AppBase *) &_result_ref;
    }
    
    resultobj = SWIG_NewPointerObj((void *) result, SWIGTYPE_p_B03AppBase, 0);
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_B03AppBase_GetRunManager(PyObject *self, PyObject *args) {
    PyObject *resultobj;
    B03AppBase *arg1 ;
    G4RunManager *result;
    PyObject * obj0  = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"O:B03AppBase_GetRunManager",&obj0)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_B03AppBase,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    {
        G4RunManager &_result_ref = (arg1)->GetRunManager();
        result = (G4RunManager *) &_result_ref;
    }
    
    resultobj = SWIG_NewPointerObj((void *) result, SWIGTYPE_p_G4RunManager, 0);
    return resultobj;
    fail:
    return NULL;
}


static PyObject * B03AppBase_swigregister(PyObject *self, PyObject *args) {
    PyObject *obj;
    if (!PyArg_ParseTuple(args,(char*)"O", &obj)) return NULL;
    SWIG_TypeClientData(SWIGTYPE_p_B03AppBase, obj);
    Py_INCREF(obj);
    return Py_BuildValue((char *)"");
}
static PyMethodDef SwigMethods[] = {
	 { (char *)"G4RunManager_BeamOn", _wrap_G4RunManager_BeamOn, METH_VARARGS },
	 { (char *)"G4RunManager_swigregister", G4RunManager_swigregister, METH_VARARGS },
	 { (char *)"G4VIStore_swigregister", G4VIStore_swigregister, METH_VARARGS },
	 { (char *)"G4VPhysicalVolume_swigregister", G4VPhysicalVolume_swigregister, METH_VARARGS },
	 { (char *)"new_G4GeometryCell", _wrap_new_G4GeometryCell, METH_VARARGS },
	 { (char *)"delete_G4GeometryCell", _wrap_delete_G4GeometryCell, METH_VARARGS },
	 { (char *)"G4GeometryCell_GetPhysicalVolume", _wrap_G4GeometryCell_GetPhysicalVolume, METH_VARARGS },
	 { (char *)"G4GeometryCell_GetReplicaNumber", _wrap_G4GeometryCell_GetReplicaNumber, METH_VARARGS },
	 { (char *)"G4GeometryCell_swigregister", G4GeometryCell_swigregister, METH_VARARGS },
	 { (char *)"new_G4IStore", _wrap_new_G4IStore, METH_VARARGS },
	 { (char *)"delete_G4IStore", _wrap_delete_G4IStore, METH_VARARGS },
	 { (char *)"G4IStore_AddImportanceGeometryCell", _wrap_G4IStore_AddImportanceGeometryCell, METH_VARARGS },
	 { (char *)"G4IStore_ChangeImportance", _wrap_G4IStore_ChangeImportance, METH_VARARGS },
	 { (char *)"G4IStore_GetImportance", _wrap_G4IStore_GetImportance, METH_VARARGS },
	 { (char *)"G4IStore_IsKnown", _wrap_G4IStore_IsKnown, METH_VARARGS },
	 { (char *)"G4IStore_GetWorldVolume", _wrap_G4IStore_GetWorldVolume, METH_VARARGS },
	 { (char *)"G4IStore_swigregister", G4IStore_swigregister, METH_VARARGS },
	 { (char *)"new_G4CellScoreValues", _wrap_new_G4CellScoreValues, METH_VARARGS },
	 { (char *)"G4CellScoreValues_fSumSL_set", _wrap_G4CellScoreValues_fSumSL_set, METH_VARARGS },
	 { (char *)"G4CellScoreValues_fSumSL_get", _wrap_G4CellScoreValues_fSumSL_get, METH_VARARGS },
	 { (char *)"G4CellScoreValues_fSumSLW_set", _wrap_G4CellScoreValues_fSumSLW_set, METH_VARARGS },
	 { (char *)"G4CellScoreValues_fSumSLW_get", _wrap_G4CellScoreValues_fSumSLW_get, METH_VARARGS },
	 { (char *)"G4CellScoreValues_fSumSLW_v_set", _wrap_G4CellScoreValues_fSumSLW_v_set, METH_VARARGS },
	 { (char *)"G4CellScoreValues_fSumSLW_v_get", _wrap_G4CellScoreValues_fSumSLW_v_get, METH_VARARGS },
	 { (char *)"G4CellScoreValues_fSumSLWE_set", _wrap_G4CellScoreValues_fSumSLWE_set, METH_VARARGS },
	 { (char *)"G4CellScoreValues_fSumSLWE_get", _wrap_G4CellScoreValues_fSumSLWE_get, METH_VARARGS },
	 { (char *)"G4CellScoreValues_fSumSLWE_v_set", _wrap_G4CellScoreValues_fSumSLWE_v_set, METH_VARARGS },
	 { (char *)"G4CellScoreValues_fSumSLWE_v_get", _wrap_G4CellScoreValues_fSumSLWE_v_get, METH_VARARGS },
	 { (char *)"G4CellScoreValues_fSumTracksEntering_set", _wrap_G4CellScoreValues_fSumTracksEntering_set, METH_VARARGS },
	 { (char *)"G4CellScoreValues_fSumTracksEntering_get", _wrap_G4CellScoreValues_fSumTracksEntering_get, METH_VARARGS },
	 { (char *)"G4CellScoreValues_fSumPopulation_set", _wrap_G4CellScoreValues_fSumPopulation_set, METH_VARARGS },
	 { (char *)"G4CellScoreValues_fSumPopulation_get", _wrap_G4CellScoreValues_fSumPopulation_get, METH_VARARGS },
	 { (char *)"G4CellScoreValues_fSumCollisions_set", _wrap_G4CellScoreValues_fSumCollisions_set, METH_VARARGS },
	 { (char *)"G4CellScoreValues_fSumCollisions_get", _wrap_G4CellScoreValues_fSumCollisions_get, METH_VARARGS },
	 { (char *)"G4CellScoreValues_fSumCollisionsWeight_set", _wrap_G4CellScoreValues_fSumCollisionsWeight_set, METH_VARARGS },
	 { (char *)"G4CellScoreValues_fSumCollisionsWeight_get", _wrap_G4CellScoreValues_fSumCollisionsWeight_get, METH_VARARGS },
	 { (char *)"G4CellScoreValues_fNumberWeightedEnergy_set", _wrap_G4CellScoreValues_fNumberWeightedEnergy_set, METH_VARARGS },
	 { (char *)"G4CellScoreValues_fNumberWeightedEnergy_get", _wrap_G4CellScoreValues_fNumberWeightedEnergy_get, METH_VARARGS },
	 { (char *)"G4CellScoreValues_fFluxWeightedEnergy_set", _wrap_G4CellScoreValues_fFluxWeightedEnergy_set, METH_VARARGS },
	 { (char *)"G4CellScoreValues_fFluxWeightedEnergy_get", _wrap_G4CellScoreValues_fFluxWeightedEnergy_get, METH_VARARGS },
	 { (char *)"G4CellScoreValues_fAverageTrackWeight_set", _wrap_G4CellScoreValues_fAverageTrackWeight_set, METH_VARARGS },
	 { (char *)"G4CellScoreValues_fAverageTrackWeight_get", _wrap_G4CellScoreValues_fAverageTrackWeight_get, METH_VARARGS },
	 { (char *)"G4CellScoreValues_fImportance_set", _wrap_G4CellScoreValues_fImportance_set, METH_VARARGS },
	 { (char *)"G4CellScoreValues_fImportance_get", _wrap_G4CellScoreValues_fImportance_get, METH_VARARGS },
	 { (char *)"G4CellScoreValues_swigregister", G4CellScoreValues_swigregister, METH_VARARGS },
	 { (char *)"new_G4CellScoreComposer", _wrap_new_G4CellScoreComposer, METH_VARARGS },
	 { (char *)"delete_G4CellScoreComposer", _wrap_delete_G4CellScoreComposer, METH_VARARGS },
	 { (char *)"G4CellScoreComposer_GetStandardCellScoreValues", _wrap_G4CellScoreComposer_GetStandardCellScoreValues, METH_VARARGS },
	 { (char *)"G4CellScoreComposer_swigregister", G4CellScoreComposer_swigregister, METH_VARARGS },
	 { (char *)"G4CellScorer_GetCellScoreComposer", _wrap_G4CellScorer_GetCellScoreComposer, METH_VARARGS },
	 { (char *)"G4CellScorer_GetCellScoreValues", _wrap_G4CellScorer_GetCellScoreValues, METH_VARARGS },
	 { (char *)"G4CellScorer_swigregister", G4CellScorer_swigregister, METH_VARARGS },
	 { (char *)"G4VCellScorerStore_swigregister", G4VCellScorerStore_swigregister, METH_VARARGS },
	 { (char *)"new_G4CellScorerStore", _wrap_new_G4CellScorerStore, METH_VARARGS },
	 { (char *)"delete_G4CellScorerStore", _wrap_delete_G4CellScorerStore, METH_VARARGS },
	 { (char *)"G4CellScorerStore_SetAutoScorerCreate", _wrap_G4CellScorerStore_SetAutoScorerCreate, METH_VARARGS },
	 { (char *)"G4CellScorerStore_AddCellScorer", _wrap_G4CellScorerStore_AddCellScorer, METH_VARARGS },
	 { (char *)"G4CellScorerStore_GetMapGeometryCellCellScorer", _wrap_G4CellScorerStore_GetMapGeometryCellCellScorer, METH_VARARGS },
	 { (char *)"G4CellScorerStore_swigregister", G4CellScorerStore_swigregister, METH_VARARGS },
	 { (char *)"G4VScorer_swigregister", G4VScorer_swigregister, METH_VARARGS },
	 { (char *)"new_G4CellStoreScorer", _wrap_new_G4CellStoreScorer, METH_VARARGS },
	 { (char *)"G4CellStoreScorer_swigregister", G4CellStoreScorer_swigregister, METH_VARARGS },
	 { (char *)"G4VImportanceAlgorithm_swigregister", G4VImportanceAlgorithm_swigregister, METH_VARARGS },
	 { (char *)"new_G4ParallelGeometrySampler", _wrap_new_G4ParallelGeometrySampler, METH_VARARGS },
	 { (char *)"delete_G4ParallelGeometrySampler", _wrap_delete_G4ParallelGeometrySampler, METH_VARARGS },
	 { (char *)"G4ParallelGeometrySampler_PrepareScoring", _wrap_G4ParallelGeometrySampler_PrepareScoring, METH_VARARGS },
	 { (char *)"G4ParallelGeometrySampler_PrepareImportanceSampling", _wrap_G4ParallelGeometrySampler_PrepareImportanceSampling, METH_VARARGS },
	 { (char *)"G4ParallelGeometrySampler_PrepareWeightRoulett", _wrap_G4ParallelGeometrySampler_PrepareWeightRoulett, METH_VARARGS },
	 { (char *)"G4ParallelGeometrySampler_Configure", _wrap_G4ParallelGeometrySampler_Configure, METH_VARARGS },
	 { (char *)"G4ParallelGeometrySampler_ClearSampling", _wrap_G4ParallelGeometrySampler_ClearSampling, METH_VARARGS },
	 { (char *)"G4ParallelGeometrySampler_IsConfigured", _wrap_G4ParallelGeometrySampler_IsConfigured, METH_VARARGS },
	 { (char *)"G4ParallelGeometrySampler_swigregister", G4ParallelGeometrySampler_swigregister, METH_VARARGS },
	 { (char *)"new_G4ScoreTable", _wrap_new_G4ScoreTable, METH_VARARGS },
	 { (char *)"delete_G4ScoreTable", _wrap_delete_G4ScoreTable, METH_VARARGS },
	 { (char *)"G4ScoreTable_Print", _wrap_G4ScoreTable_Print, METH_VARARGS },
	 { (char *)"G4ScoreTable_Write", _wrap_G4ScoreTable_Write, METH_VARARGS },
	 { (char *)"G4ScoreTable_swigregister", G4ScoreTable_swigregister, METH_VARARGS },
	 { (char *)"new_B03ImportanceDetectorConstruction", _wrap_new_B03ImportanceDetectorConstruction, METH_VARARGS },
	 { (char *)"delete_B03ImportanceDetectorConstruction", _wrap_delete_B03ImportanceDetectorConstruction, METH_VARARGS },
	 { (char *)"B03ImportanceDetectorConstruction_GetPhysicalVolumeByName", _wrap_B03ImportanceDetectorConstruction_GetPhysicalVolumeByName, METH_VARARGS },
	 { (char *)"B03ImportanceDetectorConstruction_GetWorldVolume", _wrap_B03ImportanceDetectorConstruction_GetWorldVolume, METH_VARARGS },
	 { (char *)"B03ImportanceDetectorConstruction_ListPhysNames", _wrap_B03ImportanceDetectorConstruction_ListPhysNames, METH_VARARGS },
	 { (char *)"B03ImportanceDetectorConstruction_GetGeometryCell", _wrap_B03ImportanceDetectorConstruction_GetGeometryCell, METH_VARARGS },
	 { (char *)"B03ImportanceDetectorConstruction_swigregister", B03ImportanceDetectorConstruction_swigregister, METH_VARARGS },
	 { (char *)"delete_B03AppBase", _wrap_delete_B03AppBase, METH_VARARGS },
	 { (char *)"B03AppBase_GetB03AppBase", _wrap_B03AppBase_GetB03AppBase, METH_VARARGS },
	 { (char *)"B03AppBase_GetRunManager", _wrap_B03AppBase_GetRunManager, METH_VARARGS },
	 { (char *)"B03AppBase_swigregister", B03AppBase_swigregister, METH_VARARGS },
	 { NULL, NULL }
};


/* -------- TYPE CONVERSION AND EQUIVALENCE RULES (BEGIN) -------- */

static void *_p_G4IStoreTo_p_G4VIStore(void *x) {
    return (void *)((G4VIStore *)  ((G4IStore *) x));
}
static void *_p_G4CellScorerStoreTo_p_G4VCellScorerStore(void *x) {
    return (void *)((G4VCellScorerStore *)  ((G4CellScorerStore *) x));
}
static void *_p_G4CellStoreScorerTo_p_G4VScorer(void *x) {
    return (void *)((G4VScorer *)  ((G4CellStoreScorer *) x));
}
static swig_type_info _swigt__p_G4RunManager[] = {{"_p_G4RunManager", 0, "G4RunManager *", 0},{"_p_G4RunManager"},{0}};
static swig_type_info _swigt__p_G4ScoreTable[] = {{"_p_G4ScoreTable", 0, "G4ScoreTable *", 0},{"_p_G4ScoreTable"},{0}};
static swig_type_info _swigt__p_G4VImportanceAlgorithm[] = {{"_p_G4VImportanceAlgorithm", 0, "G4VImportanceAlgorithm *", 0},{"_p_G4VImportanceAlgorithm"},{0}};
static swig_type_info _swigt__p_G4CellScoreValues[] = {{"_p_G4CellScoreValues", 0, "G4CellScoreValues *", 0},{"_p_G4CellScoreValues"},{0}};
static swig_type_info _swigt__p_G4IStore[] = {{"_p_G4IStore", 0, "G4IStore *", 0},{"_p_G4IStore"},{0}};
static swig_type_info _swigt__p_G4VIStore[] = {{"_p_G4VIStore", 0, "G4VIStore *", 0},{"_p_G4VIStore"},{"_p_G4IStore", _p_G4IStoreTo_p_G4VIStore},{0}};
static swig_type_info _swigt__p_G4ParallelGeometrySampler[] = {{"_p_G4ParallelGeometrySampler", 0, "G4ParallelGeometrySampler *", 0},{"_p_G4ParallelGeometrySampler"},{0}};
static swig_type_info _swigt__p_G4CellScorerStore[] = {{"_p_G4CellScorerStore", 0, "G4CellScorerStore *", 0},{"_p_G4CellScorerStore"},{0}};
static swig_type_info _swigt__p_G4VCellScorerStore[] = {{"_p_G4VCellScorerStore", 0, "G4VCellScorerStore *", 0},{"_p_G4VCellScorerStore"},{"_p_G4CellScorerStore", _p_G4CellScorerStoreTo_p_G4VCellScorerStore},{0}};
static swig_type_info _swigt__p_B03AppBase[] = {{"_p_B03AppBase", 0, "B03AppBase *", 0},{"_p_B03AppBase"},{0}};
static swig_type_info _swigt__p_G4std__ostream[] = {{"_p_G4std__ostream", 0, "G4std::ostream *", 0},{"_p_G4std__ostream"},{0}};
static swig_type_info _swigt__p_G4CellStoreScorer[] = {{"_p_G4CellStoreScorer", 0, "G4CellStoreScorer *", 0},{"_p_G4CellStoreScorer"},{0}};
static swig_type_info _swigt__p_G4CellScoreComposer[] = {{"_p_G4CellScoreComposer", 0, "G4CellScoreComposer *", 0},{"_p_G4CellScoreComposer"},{0}};
static swig_type_info _swigt__p_G4GeometryCell[] = {{"_p_G4GeometryCell", 0, "G4GeometryCell *", 0},{"_p_G4GeometryCell"},{0}};
static swig_type_info _swigt__p_G4MapGeometryCellCellScorer[] = {{"_p_G4MapGeometryCellCellScorer", 0, "G4MapGeometryCellCellScorer const &", 0},{"_p_G4MapGeometryCellCellScorer"},{0}};
static swig_type_info _swigt__p_G4CellScorer[] = {{"_p_G4CellScorer", 0, "G4CellScorer *", 0},{"_p_G4CellScorer"},{0}};
static swig_type_info _swigt__p_G4VScorer[] = {{"_p_G4VScorer", 0, "G4VScorer *", 0},{"_p_G4VScorer"},{"_p_G4CellStoreScorer", _p_G4CellStoreScorerTo_p_G4VScorer},{0}};
static swig_type_info _swigt__p_G4bool[] = {{"_p_G4bool", 0, "G4bool *", 0},{"_p_G4bool"},{0}};
static swig_type_info _swigt__p_G4VPhysicalVolume[] = {{"_p_G4VPhysicalVolume", 0, "G4VPhysicalVolume *", 0},{"_p_G4VPhysicalVolume"},{0}};
static swig_type_info _swigt__p_G4double[] = {{"_p_G4double", 0, "G4double *", 0},{"_p_G4double"},{0}};
static swig_type_info _swigt__p_B03ImportanceDetectorConstruction[] = {{"_p_B03ImportanceDetectorConstruction", 0, "B03ImportanceDetectorConstruction *", 0},{"_p_B03ImportanceDetectorConstruction"},{0}};

static swig_type_info *swig_types_initial[] = {
_swigt__p_G4RunManager, 
_swigt__p_G4ScoreTable, 
_swigt__p_G4VImportanceAlgorithm, 
_swigt__p_G4CellScoreValues, 
_swigt__p_G4IStore, 
_swigt__p_G4VIStore, 
_swigt__p_G4ParallelGeometrySampler, 
_swigt__p_G4CellScorerStore, 
_swigt__p_G4VCellScorerStore, 
_swigt__p_B03AppBase, 
_swigt__p_G4std__ostream, 
_swigt__p_G4CellStoreScorer, 
_swigt__p_G4CellScoreComposer, 
_swigt__p_G4GeometryCell, 
_swigt__p_G4MapGeometryCellCellScorer, 
_swigt__p_G4CellScorer, 
_swigt__p_G4VScorer, 
_swigt__p_G4bool, 
_swigt__p_G4VPhysicalVolume, 
_swigt__p_G4double, 
_swigt__p_B03ImportanceDetectorConstruction, 
0
};


/* -------- TYPE CONVERSION AND EQUIVALENCE RULES (END) -------- */

static swig_const_info swig_const_table[] = {
{0}};

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C"
#endif
SWIGEXPORT(void) SWIG_init(void) {
    static PyObject *SWIG_globals = 0; 
    static int       typeinit = 0;
    PyObject *m, *d;
    int       i;
    if (!SWIG_globals) SWIG_globals = SWIG_newvarlink();
    m = Py_InitModule((char *) SWIG_name, SwigMethods);
    d = PyModule_GetDict(m);
    
    if (!typeinit) {
        for (i = 0; swig_types_initial[i]; i++) {
            swig_types[i] = SWIG_TypeRegister(swig_types_initial[i]);
        }
        typeinit = 1;
    }
    SWIG_InstallConstants(d,swig_const_table);
    
}

