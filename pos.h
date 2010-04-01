#include <Python.h>
#include <structmember.h>

#if PY_VERSION_HEX < 0x02050000 && !defined(PY_SSIZE_T_MIN)
typedef int Py_ssize_t;
#define PY_SSIZE_T_MAX INT_MAX
#define PY_SSIZE_T_MIN INT_MIN
#endif

#define POSLEN 3


typedef struct PosObject {
	PyObject_HEAD
	double elements[POSLEN];
} PosObject;


#define Pos_Check(op) PyObject_TypeCheck(op, &PosObjectType)


// internal functions
PyObject* pos_get_element(PyObject* self_in, long index);

// Python API functions
int Pos_init(PosObject *self, PyObject *args, PyObject *kwds);
PyObject* Pos_getx(PyObject* self_in, void* closure);
PyObject* Pos_gety(PyObject* self_in, void* closure);
PyObject* Pos_getz(PyObject* self_in, void* closure);
int Pos_set_notallowed(PyObject* self_in, PyObject* value, void* closure);
PyObject* Pos_repr(PyObject *self_in);
int Pos_true(PyObject *self_in);
PyObject* Pos_add(PyObject *self_in, PyObject *other_in);
PyObject* Pos_sub(PyObject *self_in, PyObject *other_in);
PyObject* Pos_ip_add(PyObject *self_in, PyObject *other_in);
PyObject* Pos_ip_sub(PyObject *self_in, PyObject *other_in);
PyObject* Pos_ip_zero(PyObject *self_in, PyObject *unused);
PyObject* Pos_average(PyObject *self_in, PyObject *args);
PyObject* Pos_copy(PyObject *self_in, PyObject *unused);
PyObject* Pos_dist(PyObject *self_in, PyObject *args);
PyObject* Pos_slerp(PyObject *self_in, PyObject *args);
PyObject* Pos_sserp(PyObject *self_in, PyObject *args);
Py_ssize_t Pos_len(PyObject *self_in);
PyObject* Pos_item(PyObject *self_in, Py_ssize_t index);





extern PyNumberMethods Pos_as_number[];
extern PySequenceMethods Pos_as_seq[];
extern PyGetSetDef Pos_getset[];
extern PyMethodDef Pos_methods[];
extern struct PyMemberDef Pos_members[];
extern PyTypeObject PosObjectType;
