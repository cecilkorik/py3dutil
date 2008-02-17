#include <Python.h>
#include <structmember.h>

#if PY_VERSION_HEX < 0x02050000 && !defined(PY_SSIZE_T_MIN)
typedef int Py_ssize_t;
#define PY_SSIZE_T_MAX INT_MAX
#define PY_SSIZE_T_MIN INT_MIN
#endif

typedef struct rb_red_blk_tree rb_red_blk_tree;
typedef struct ObarrObject ObarrObject;

typedef struct CgridObject {
	PyObject_HEAD
	rb_red_blk_tree*	pTree;
	long				nSize;
	long				nCells;
	double				dCellSize;
	ObarrObject*		pUnrolled;
	int					bUnrollDirty;
} CgridObject;

staticforward PyTypeObject CgridObjectType;

#define Cgrid_Check(op) PyObject_TypeCheck(op, &CgridObjectType)
#define COPY_KEY(a, b) (b)->x = (a)->x; (b)->y = (a)->y; (b)->z = (a)->z;

typedef struct CgridKey {
	long x;
	long y;
	long z;
} CgridKey;
typedef struct CgridInfo {
	CgridObject* pSelf;
	CgridKey k;
	ObarrObject* pContents;
	
} CgridInfo;

#define SQR(x) ((x) * (x))

/* internal functions */
void cgrid_unroll(CgridObject* self);
int cgrid_compare(const void* a, const void* b); 
CgridKey* cgrid_newkey();
CgridInfo* cgrid_newinfo();
void cgrid_destroykey(void* a);
void cgrid_destroyinfo(void* a);
void cgrid_printkey(const void* a);
void cgrid_printinfo(void* a);
long cgrid_coord_to_gridcoord(CgridObject* self, double coord);
ObarrObject* cgrid_get_radius(CgridObject* self, PyObject* other, double dRadius);
void cgrid_get_radius_append(CgridObject* self, PyObject* other, double dRadius, ObarrObject* pNeighbors);

/* exported API functions */
int Cgrid_init(CgridObject *self, PyObject *args, PyObject *kwds);
void Cgrid_dealloc(PyObject* self_in);
int Cgrid_contains(PyObject* self_in, PyObject* other_in);
PyObject * Cgrid_item(PyObject *self_in, Py_ssize_t index);
Py_ssize_t Cgrid_len(PyObject *self_in);
PyObject* Cgrid_repr(PyObject *self_in);
PyObject* Cgrid_insert(PyObject *self_in, PyObject *args);
PyObject* Cgrid_delete(PyObject *self_in, PyObject *args);
PyObject* Cgrid_remove(PyObject *self_in, PyObject *args);
PyObject* Cgrid_get_radius(PyObject *self_in, PyObject *args);
/*PyObject* Cgrid_remove(PyObject *self_in, PyObject *args);*/

extern PySequenceMethods Cgrid_as_seq[];
extern PyMethodDef Cgrid_methods[];
extern struct PyMemberDef Cgrid_members[];
extern PyTypeObject CgridObjectType;