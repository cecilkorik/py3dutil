#include <Python.h>
#include <structmember.h>

#if PY_VERSION_HEX < 0x02050000 && !defined(PY_SSIZE_T_MIN)
typedef int Py_ssize_t;
#define PY_SSIZE_T_MAX INT_MAX
#define PY_SSIZE_T_MIN INT_MIN
#endif

typedef struct rb_red_blk_tree rb_red_blk_tree;
typedef struct ObarrObject ObarrObject;

typedef struct ColliderObject {
	PyObject_HEAD
	ObarrObject*	pGrids;
} ColliderObject;

extern PyTypeObject ColliderObjectType;

#define Collider_Check(op) PyObject_TypeCheck(op, &ColliderObjectType)


double sizes[];

#define SQR(x) ((x) * (x))

/* internal functions */


/* exported API functions */
int Collider_init(ColliderObject *self, PyObject *args, PyObject *kwds);
void Collider_dealloc(PyObject* self_in);
PyObject* Collider_begin(PyObject *self_in, PyObject *args);
PyObject* Collider_remove(PyObject *self_in, PyObject *args);

extern PySequenceMethods Collider_as_seq[];
extern PyMethodDef Collider_methods[];
extern struct PyMemberDef Collider_members[];
