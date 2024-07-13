#include "Python.h"
#include "frameobject.h"
#include "pystate.h"

/**
 * This file is part of the IKPdb Debugger
 * Copyright (c) 2016-2018 by cyril MORISSE, Audaxis
 * Licence: MIT. See LICENCE at repository root
 */

static long debuggerThreadIdent = 0;  // Track debugger thread ident


/* 
 * Redefine needed static functions from Python-3.x.x/Python/sysmodule.c
 */

/*
 * Cached interned string objects used for calling the profile and
 * trace functions.  Initialized by trace_init().
 */
static PyObject *whatstrings[7] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL};

static int trace_init(void)
{
    static const char * const whatnames[7] = {
        "call", "exception", "line", "return",
        "c_call", "c_exception", "c_return"
    };
    PyObject *name;
    int i;
    for (i = 0; i < 7; ++i) {
        if (whatstrings[i] == NULL) {
            name = PyUnicode_InternFromString(whatnames[i]);
            if (name == NULL)
                return -1;
            whatstrings[i] = name;
        }
    }
    return 0;
}

static PyObject *
get_frame_trace(PyFrameObject *frame)
{
    PyObject *trace_func = NULL;
    PyObject *locals_dict = PyFrame_GetLocals(frame);
    if (locals_dict && PyDict_Check(locals_dict)) {
        trace_func = PyDict_GetItemString(locals_dict, "__tracefunc__");
        Py_XINCREF(trace_func);
    }
    return trace_func;
}

static void
set_frame_trace(PyFrameObject *frame, PyObject *trace_func)
{
    PyObject *locals_dict = PyFrame_GetLocals(frame);
    if (locals_dict && PyDict_Check(locals_dict)) {
        if (trace_func) {
            PyDict_SetItemString(locals_dict, "__tracefunc__", trace_func);
        } else {
            PyDict_DelItemString(locals_dict, "__tracefunc__");
        }
    }

    Py_XDECREF(trace_func);
}

static PyObject *
call_trampoline(PyObject* callback,
                PyFrameObject *frame, int what, PyObject *arg)
{
    PyObject *result;
    PyObject *stack[3];

    if (PyFrame_FastToLocalsWithError(frame) < 0) {
        return NULL;
    }

    stack[0] = (PyObject *)frame;
    stack[1] = whatstrings[what];
    stack[2] = (arg != NULL) ? arg : Py_None;
    /* call the Python-level function */
    result = _PyObject_FastCall(callback, stack, 3);

    PyFrame_LocalsToFast(frame, 1);
    if (result == NULL) {
        PyTraceBack_Here(frame);
    }

    return result;
}

static int
trace_trampoline(PyObject *self, PyFrameObject *frame,
                 int what, PyObject *arg)
{
    PyObject *callback;
    PyObject *result;

    if (what == PyTrace_CALL)
        callback = self;
    else
        callback = get_frame_trace(frame);

    if (callback == NULL)
        return 0;

    result = call_trampoline(callback, frame, what, arg);
    set_frame_trace(frame, result);

    if (result == NULL) {
        PyEval_SetTrace(NULL, NULL);
        return -1;
    }

    return 0;
}

/* 
 * iksettrace3 'real' functions
 */

/*
* Based on https://docs.python.org/3/whatsnew/3.11.html#whatsnew311-c-api-porting
*/
static inline void
IK_UseTracing(PyThreadState *tstate, int use_tracing)
{
    tstate->tracing += use_tracing ? 1 : -1;
#if PY_VERSION_HEX >= 0x030A00A1
    tstate->cframe->use_tracing = use_tracing ? 255 : 0;
#else
    tstate->use_tracing = use_tracing;
#endif
}

void
IK_SetTrace(Py_tracefunc func, PyObject *arg)
{
    // Ensure _Py_TracingPossible is correctly set
    PyEval_SetTrace(func, arg);  
    
    // Now iterate over all threads to set tracing
    PyInterpreterState *interp = PyInterpreterState_Head();
    PyThreadState *loopThreadState = PyInterpreterState_ThreadHead(interp);
    while(loopThreadState) {
        if ((unsigned long)loopThreadState->thread_id != (unsigned long)debuggerThreadIdent) {
            PyObject *temp = loopThreadState->c_traceobj;
            Py_XINCREF(arg);
            loopThreadState->c_tracefunc = NULL;
            loopThreadState->c_traceobj = NULL;
            /* Must make sure that profiling is not ignored if 'temp' is freed */
            IK_UseTracing(loopThreadState, loopThreadState->c_profilefunc != NULL);
            Py_XDECREF(temp);
            loopThreadState->c_tracefunc = func;
            loopThreadState->c_traceobj = arg;
            /* Flag that tracing or profiling is turned on */
            IK_UseTracing(loopThreadState, (func != NULL) || (loopThreadState->c_profilefunc != NULL));
        } else {
            PyObject *temp = loopThreadState->c_traceobj;
            loopThreadState->c_tracefunc = NULL;
            loopThreadState->c_traceobj = NULL;
            /* Must make sure that profiling is not ignored if 'temp' is freed */
            IK_UseTracing(loopThreadState, loopThreadState->c_profilefunc != NULL);
            Py_XDECREF(temp);

        };
        loopThreadState = PyThreadState_Next(loopThreadState);
    };
}


static PyObject *
_ik_set_trace_on(PyObject *self, PyObject *args)
{
    PyObject *traceObject = NULL;

    if (trace_init() == -1)
        return NULL;
    
    if (!PyArg_ParseTuple(args, "Ol", &traceObject, &debuggerThreadIdent)) {
        return NULL;
    }    

    IK_SetTrace(trace_trampoline, traceObject);

    Py_INCREF(Py_None);
    return Py_None;
}

PyDoc_STRVAR(_ik_set_trace_on_doc,
"_set_trace_on(tracer, thread_id)\n\
\n\
Activate tracing with tracer function, on all threads but the one specified.\n\
See the debugger chapter in the library manual.\n\
This function do not call threading.settrace(), user must do it."
);


static PyObject *
_ik_set_trace_off(PyObject *self)
{
    IK_SetTrace(NULL, NULL);
    Py_INCREF(Py_None);
    return Py_None;
}

PyDoc_STRVAR(_ik_set_trace_off_doc,
"_set_trace_off()\n\
\n\
Disable tracing on all threads.\n\
See the debugger chapter in the library manual.\n\
This function do not call threading.settrace(), user must do it."
);


static PyMethodDef InoukMethods[] = {
    {"_set_trace_on", _ik_set_trace_on, METH_VARARGS, _ik_set_trace_on_doc},
    {"_set_trace_off", (PyCFunction)_ik_set_trace_off, METH_NOARGS, _ik_set_trace_off_doc},
    {NULL, NULL, 0, NULL}           /* sentinel */
};

PyDoc_STRVAR(iksettrace3_module_doc,
"iksettrace3 module\n\
\n\
Contains 2 functions that allows to enable and disable tracing on all threads (even existing one).\n\
Note that functions in this module do not call threading.settrace() to set trace method on future threads.\n\
Module user must do it.");



static struct PyModuleDef iksettrace3_module = {
   PyModuleDef_HEAD_INIT,
   "iksettrace3",   /* name of module */
   iksettrace3_module_doc, /* module documentation, may be NULL */
   -1,       /* size of per-interpreter state of the module,
                or -1 if the module keeps state in global variables. */
   InoukMethods
};

PyMODINIT_FUNC
PyInit_iksettrace3(void)
{
    return PyModule_Create(&iksettrace3_module);
}
