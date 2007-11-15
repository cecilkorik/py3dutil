#include <Python.h>
#include <structmember.h>

#if PY_VERSION_HEX < 0x02050000 && !defined(PY_SSIZE_T_MIN)
typedef int Py_ssize_t;
#define PY_SSIZE_T_MAX INT_MAX
#define PY_SSIZE_T_MIN INT_MIN
#endif

#define VEC3D

#ifdef VEC3D
#define VECLEN 3
#endif

#ifdef VEC2D
#define VECLEN 2
#endif


typedef struct VectObject {
	PyObject_HEAD
	double elements[VECLEN];
} VectObject;


#define Vect_Check(op) PyObject_TypeCheck(op, &VectObjectType)


// internal functions
PyObject* vect_get_element(PyObject* self_in, long index);

// Python API functions
int Vect_init(VectObject *self, PyObject *args, PyObject *kwds);
PyObject* Vect_getx(PyObject* self_in, void* closure);
PyObject* Vect_gety(PyObject* self_in, void* closure);
#ifdef VEC3D
PyObject* Vect_getz(PyObject* self_in, void* closure);
#endif
int Vect_set_notallowed(PyObject* self_in, PyObject* value, void* closure);
PyObject* Vect_repr(PyObject *self_in);
int Vect_true(PyObject *self_in);
PyObject* Vect_add(PyObject *self_in, PyObject *other_in);
PyObject* Vect_sub(PyObject *self_in, PyObject *other_in);
PyObject* Vect_mul(PyObject *self_in, PyObject *other_in);
PyObject* Vect_div(PyObject *self_in, PyObject *other_in);
PyObject* Vect_ip_add(PyObject *self_in, PyObject *other_in);
PyObject* Vect_ip_sub(PyObject *self_in, PyObject *other_in);
PyObject* Vect_ip_mul(PyObject *self_in, PyObject *other_in);
PyObject* Vect_ip_div(PyObject *self_in, PyObject *other_in);
PyObject* Vect_negate(PyObject *self_in);
PyObject* Vect_ip_negate(PyObject *self_in, PyObject *unused);
PyObject* Vect_ip_zero(PyObject *self_in, PyObject *unused);
PyObject* Vect_ip_normalize(PyObject *self_in, PyObject *unused);
PyObject* Vect_mag(PyObject *self_in, PyObject *unused);
PyObject* Vect_mag2(PyObject *self_in, PyObject *unused);
PyObject* Vect_dotprod(PyObject *self_in, PyObject *args);
PyObject* Vect_crossprod(PyObject *self_in, PyObject *unused);
PyObject* Vect_average(PyObject *self_in, PyObject *args);
PyObject* Vect_dir(PyObject *self_in, PyObject *unused);
PyObject* Vect_copy(PyObject *self_in, PyObject *unused);
PyObject* Vect_dist(PyObject *self_in, PyObject *args);
PyObject* Vect_slerp(PyObject *self_in, PyObject *args);
PyObject* Vect_sserp(PyObject *self_in, PyObject *args);
Py_ssize_t Vect_len(PyObject *self_in);
PyObject* Vect_item(PyObject *self_in, Py_ssize_t index);





extern PyNumberMethods Vect_as_number[];
extern PySequenceMethods Vect_as_seq[];
extern PyGetSetDef Vect_getset[];
extern PyMethodDef Vect_methods[];
extern struct PyMemberDef Vect_members[];
extern PyTypeObject VectObjectType;
