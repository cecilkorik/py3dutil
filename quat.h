#include <Python.h>
#include <structmember.h>
#include "vect.h"

#if PY_VERSION_HEX < 0x02050000 && !defined(PY_SSIZE_T_MIN)
typedef int Py_ssize_t;
#define PY_SSIZE_T_MAX INT_MAX
#define PY_SSIZE_T_MIN INT_MIN
#endif

typedef struct QuatObject {
	PyObject_HEAD
	double elements[4];
} QuatObject;


#define Quat_Check(op) PyObject_TypeCheck(op, &QuatObjectType)

// internal functions
PyObject* quat_get_element(PyObject* self_in, long index);
void quat_multiply_internal(QuatObject* q1, QuatObject* q2, QuatObject* qr);
void quat_multiply_vect_internal(QuatObject* self, VectObject* v, VectObject* rv);
double quat_mag_internal(QuatObject* self);
double quat_mag2_internal(QuatObject* self);
void quat_normalize_internal(QuatObject* self);
void quat_get_conjugate_internal(QuatObject* self, QuatObject* rv);

// Python API functions
int Quat_init(QuatObject *self, PyObject *args, PyObject *kwds);
PyObject* Quat_getx(PyObject* self_in, void* closure);
PyObject* Quat_gety(PyObject* self_in, void* closure);
PyObject* Quat_getz(PyObject* self_in, void* closure);
PyObject* Quat_getw(PyObject* self_in, void* closure);
int Quat_set_notallowed(PyObject* self_in, PyObject* value, void* closure);
PyObject* Quat_repr(PyObject *self_in);
int Quat_true(PyObject *self_in);
PyObject* Quat_mul(PyObject *self_in, PyObject *other_in);
PyObject* Quat_ip_normalize(PyObject *self_in, PyObject *unused);
PyObject* Quat_mag(PyObject *self_in, PyObject *unused);
PyObject* Quat_mag2(PyObject *self_in, PyObject *unused);
PyObject* Quat_copy(PyObject *self_in, PyObject *unused);
PyObject* Quat_get_angle(PyObject *self_in, PyObject *unused);
PyObject* Quat_get_conjugate(PyObject *self_in, PyObject *unused);
PyObject* Quat_get_matrix(PyObject *self_in, PyObject *unused);
PyObject* Quat_slerp(PyObject *self_in, PyObject *args);
PyObject* Quat_slerp_turn(PyObject *self_in, PyObject *args);
Py_ssize_t Quat_len(PyObject *self_in);
PyObject* Quat_item(PyObject *self_in, Py_ssize_t index);





extern PyNumberMethods Quat_as_number[];
extern PySequenceMethods Quat_as_seq[];
extern PyGetSetDef Quat_getset[];
extern PyMethodDef Quat_methods[];
extern struct PyMemberDef Quat_members[];
extern PyTypeObject QuatObjectType;
