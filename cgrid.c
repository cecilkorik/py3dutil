#include "cgrid.h"
#include "obarr.h"
#include "red_black_tree.h"
#include <math.h>

void cgrid_unroll(CgridObject* self)
{
	long i = 0;
	rb_red_blk_node* nil = self->pTree->nil;
	rb_red_blk_node* x = self->pTree->root->left;
	rb_red_blk_node* last = nil;
	
	if (!self->bUnrollDirty)
		return;
		
	if (self->pUnrolled)
	{
		Py_DECREF(self->pUnrolled);
	}
	self->pUnrolled = PyObject_New(ObarrObject, &ObarrObjectType);
	obarr_set_size(self->pUnrolled, self->nCells);
	
	while(nil != x) {
		last = x;
		x = x->right;
	}
	while (last != nil) {
		if (i == self->nCells)
		{
			printf("SizeError in Unroll!\n");
			break;
		}
		obarr_set_element(self->pUnrolled, i++, (PyObject*)last->info);
		last = TreePredecessor(self->pTree,last);
	}
	self->bUnrollDirty = 0;
}

int cgrid_compare(const void* a, const void* b)
{
	const CgridKey* sa = (const CgridKey*)a;
	const CgridKey* sb = (const CgridKey*)b;
	int rv;
	
	rv = (sa->x > sb->x) - (sa->x < sb->x);
	if (rv != 0)
	{
		return rv;
	}
	else
	{
		rv = (sa->y > sb->y) - (sa->y < sb->y);
		if (rv != 0)
		{
			return rv;
		}
		else
		{
			rv = (sa->z > sb->z) - (sa->z < sb->z);
			return rv;
		}
	}
	return 0;
}


CgridKey* cgrid_newkey()
{
	CgridKey* ptr;
	
	ptr = (CgridKey*)malloc(sizeof(CgridKey));
	return ptr;
}
CgridInfo* cgrid_newinfo()
{
	CgridInfo* ptr;
	
	ptr = (CgridInfo*)malloc(sizeof(CgridInfo));
	return ptr;
}


void cgrid_destroykey(void* a)
{
	free(a);
}

void cgrid_destroyinfo(void* a)
{
	const CgridInfo* sa = (const CgridInfo*)a;
	PyObject** other = sa->pContents->pData;	
	
	for (;other - sa->pContents->pData < sa->pContents->nSize; other++)
	{
		if (*other == Py_None)
			continue;
		PyObject_DelAttrString(*other, "_cgrid_internal_key_x");
		PyObject_DelAttrString(*other, "_cgrid_internal_key_y");
		PyObject_DelAttrString(*other, "_cgrid_internal_key_z");
	}

	obarr_empty(sa->pContents);
	Py_DECREF(sa->pContents);
	free(a);
}

void cgrid_printkey(const void* a)
{
	const CgridKey* sa = (const CgridKey*)a;
	
	printf("<GridKey(%d, %d, %d)>", sa->x, sa->y, sa->z);
}

void cgrid_printinfo(void* a)
{
	ObarrObject* arr = (ObarrObject*)a;
	
	printf("<Obarr size %d>", arr->nSize);
}

long cgrid_coord_to_gridcoord(CgridObject* self, double coord)
{
	return long(floor(coord / self->dCellSize));
}

ObarrObject* cgrid_get_radius(CgridObject *self, PyObject *other, double dRadius)
{
	ObarrObject *pNeighbors;
	pNeighbors = PyObject_New(ObarrObject, &ObarrObjectType);
	if (!cgrid_get_radius_append(pNeighbors))
	{
		Py_DECREF(pNeighbors);
		return NULL;
	}
	return pNeighbors;

}
void* cgrid_get_radius_append(CgridObject *self, PyObject *other, double dRadius, ObarrObject *pNeighbors)
{
	PyObject *el = NULL;
	rb_red_blk_node* pNode = NULL;
	PyObject *pAttr;
	CgridInfo *pV;
	CgridKey k;
	double dRe;
	double dX, dY, dZ;
	double dXi, dYi, dZi;
	double dXs, dYs, dZs;
	double dXe, dYe, dZe;
	long x, y, r, i;
	
    if (!PyArg_ParseTuple(args, "Od", &other, &dRadius))
	{
		PyErr_SetString(PyExc_TypeError, "wrong arguments");
		return NULL;
	}
	
	pAttr = PyObject_GetAttrString(other, "pos")
    if (!pAttr)
    {
		PyErr_SetString(PyExc_TypeError, "missing 'pos' attribute of input object");
		return NULL;
    }
    
    if (!PyArg_ParseTuple(pAttr, "ddd", &dX, &dY, &dZ))
	{
		Py_DECREF(pAttr);
		PyErr_SetString(PyExc_TypeError, "invalid 'pos' attribute of input object");
		return NULL;
	}
	Py_DECREF(pAttr);

	dXs = dX + dRadius + (self->dCellSize * 0.1);
	dYs = dY + dRadius + (self->dCellSize * 0.1);
	dZs = dZ + dRadius + (self->dCellSize * 0.1);
	
	for (dXi = (dX - dRadius); dXi <= dXs; dXi += self->dCellSize)
	{
		for (dYi = (dY - dRadius); dYi <= dYs; dYi += self->dCellSize)
		{
			for (dZi = (dZ - dRadius); dZi <= dZs; dZi += self->dCellSize)
			{
				k.x = cgrid_coord_to_gridcoord(dXi);
				k.y = cgrid_coord_to_gridcoord(dYi);
				k.z = cgrid_coord_to_gridcoord(dZi);
				pNode = RBExactQuery(self->pTree, &k);
				if (pNode)
				{
					pV = (CgridInfo*)pNode->info;
					for (i = 0; i < pV->pContents->nSize; i++)
					{
						el = obarr_get_element(pV->pContents, i);
						pAttr = PyObject_GetAttrString(other, "pos");
					    if (!pAttr)
						{
							PyErr_SetString(PyExc_TypeError, "missing 'pos' attribute of object in grid");
							return NULL;
						}
					    if (!PyArg_ParseTuple(pAttr, "ddd", &dXe, &dYe, &dZe))
						{
							Py_DECREF(pAttr);
							PyErr_SetString(PyExc_TypeError, "invalid 'pos' attribute of object in grid");
							return NULL;
						}
						Py_DECREF(pAttr);

						pAttr = PyObject_GetAttrString(other, "radius");
					    if (!pAttr)
							dRe = 0.0;
					    else if (!PyFloat_Check(pAttr))
						{
							Py_DECREF(pAttr);
							PyErr_SetString(PyExc_TypeError, "invalid 'radius' attribute of object in grid");
							return NULL;
						}
						else
							dRe = PyFloat_AsDouble(pAttr);
						Py_DECREF(pAttr);
						
						dDist = sqrt(SQR(dXe - dX) + SQR(dYe - dY) + SQR(dZe - dZ)) - dRe;
						if (dDist > dRadius)
							continue;
						if (!obarr_append(pNeighbors, el))
						{
							PyErr_SetString(PyExc_MemoryError, "out of memory");
							return NULL;
						}
					}
				}
			}
		}
	}
	
	return pNeighbors;
}


int Cgrid_init(CgridObject *self, PyObject *args, PyObject *kwds)
{
	double dCell;
	
    if (!PyArg_ParseTuple(args, "d", &dCell))
	{
		PyErr_SetString(PyExc_TypeError, "wrong arguments");
		return 1;
	}
	
	self->pTree = RBTreeCreate(cgrid_compare, cgrid_destroykey, cgrid_destroyinfo, cgrid_printkey, cgrid_printinfo);
	self->nSize = 0;
	self->nCells = 0;
	self->dCellSize = dCell;
	self->pUnrolled = NULL;
	self->bUnrollDirty = 1;

	return 0;
}

void Cgrid_dealloc(PyObject* self_in)
{
	CgridObject* self = (CgridObject*)self_in;
	
	RBTreeDestroy(self->pTree);
}

PyObject* Cgrid_repr(PyObject *self_in)
{
	CgridObject *self;
	PyObject *tuple, *fmtstring, *reprstring;
	
	if (!Cgrid_Check(self_in))
		return PyString_FromString("<unknown object type>");
	
	self = (CgridObject*)self_in;
	tuple = Py_BuildValue("(ii)", self->nCells, self->nSize);
	fmtstring = PyString_FromString("<cgrid of %d cells, %d objects>");
	reprstring = PyString_Format(fmtstring, tuple);
	Py_DECREF(tuple);
	Py_DECREF(fmtstring);
	return reprstring;
}

/*
PyObject *
Cgrid_copy(PyObject *self_in, PyObject *unused)
{
	if (!Cgrid_Check(self_in))	
	{
		PyErr_SetString(PyExc_TypeError, "not a vector");
		return NULL;
	}
	CgridObject* self = (CgridObject*)self_in;
	CgridObject* rv = NULL;
	rv = PyObject_New(CgridObject, &CgridObjectType);
	int i;
	for (i = 0; i < VECLEN; i++)
		rv->elements[i] = self->elements[i];

	return (PyObject*)rv;
}
*/
Py_ssize_t Cgrid_len(PyObject *self_in)
{
	CgridObject* self = (CgridObject*)self_in;
	
	return self->nCells;
}

PyObject* Cgrid_item(PyObject *self_in, Py_ssize_t index)
{
	CgridObject* self = (CgridObject*)self_in;
	
	cgrid_unroll(self);
	if (!obarr_valid_index(self->pUnrolled, index))
	{
		PyErr_SetString(PyExc_IndexError, "invalid index");
		return NULL;
	}
	return obarr_get_element(self->pUnrolled, index);
}

int Cgrid_contains(PyObject* self_in, PyObject* other_in)
{
	CgridObject* self = (CgridObject*)self_in;
	rb_red_blk_node* pNode = NULL;
	CgridKey k;
	
    if (!PyArg_ParseTuple(other_in, "lll", &k.x, &k.y, &k.z))
	{
		PyErr_SetString(PyExc_TypeError, "wrong arguments");
		return 0;
	}

	pNode = RBExactQuery(self->pTree, &k);
	return !!pNode;
}

PyObject* Cgrid_insert(PyObject *self_in, PyObject *args)
{
	CgridObject* self = (CgridObject*)self_in;
	PyObject* other = NULL;
	CgridKey* pK = NULL;
	CgridInfo* pV = NULL;
	rb_red_blk_node* pNode = NULL;
	
	pK = cgrid_newkey();
    if (!PyArg_ParseTuple(args, "(lll)O", &pK->x, &pK->y, &pK->z, &other))
	{
		free(pK);
		PyErr_SetString(PyExc_TypeError, "wrong arguments");
		return NULL;
	}
	
	PyObject_SetAttrString(other, "_cgrid_internal_key_x", PyInt_FromLong(pK->x));
	PyObject_SetAttrString(other, "_cgrid_internal_key_y", PyInt_FromLong(pK->y));
	PyObject_SetAttrString(other, "_cgrid_internal_key_z", PyInt_FromLong(pK->z));
	
	pNode = RBExactQuery(self->pTree, pK);
	if (pNode)
	{
		pV = (CgridInfo*)pNode->info;
	}
	else
	{
		pV = cgrid_newinfo();
		pV->pSelf = self;
		pV->pContents = PyObject_New(ObarrObject, &ObarrObjectType);
		COPY_KEY(pK, &(pV->k));

		RBTreeInsert(self->pTree, pK, pV);
		self->nCells++;
	}
	obarr_set_size(pV->pContents, pV->pContents->nSize + 1);
	obarr_set_element(pV->pContents, pV->pContents->nSize - 1, other);
	
	self->nSize++;
	self->bUnrollDirty = 1;
	Py_INCREF(Py_None);
	return Py_None;

}

PyObject* Cgrid_delete(PyObject *self_in, PyObject *args)
{
	CgridObject* self = (CgridObject*)self_in;
	rb_red_blk_node* pNode = NULL;
	CgridInfo* pV;
	CgridKey k;
	
    if (!PyArg_ParseTuple(args, "(lll)", &k.x, &k.y, &k.z))
	{
		PyErr_SetString(PyExc_TypeError, "wrong arguments");
		return NULL;
	}

	pNode = RBExactQuery(self->pTree, &k);
	if (!pNode)
	{
		PyErr_SetString(PyExc_ValueError, "supplied argument not found in grid (no such cell)");
		return NULL;
	}
	pV = (CgridInfo*)pNode->info;
	self->nSize = self->nSize - pV->pContents->nSize;
	RBDelete(self->pTree, pNode);
	self->nCells--;

	self->bUnrollDirty = 1;
	
	Py_INCREF(Py_None);
	return Py_None;

}

PyObject* Cgrid_remove(PyObject *self_in, PyObject *args)
{
	CgridObject* self = (CgridObject*)self_in;
	PyObject *other, *element;
	CgridKey k;
	CgridInfo* pV;
	PyObject *pX, *pY, *pZ;
	rb_red_blk_node* pNode = NULL;
	long i;

    if (!PyArg_ParseTuple(args, "O", &other))
	{
		PyErr_SetString(PyExc_TypeError, "wrong arguments");
		return NULL;
	}
	
	pX = PyObject_GetAttrString(other, "_cgrid_internal_key_x");
	pY = PyObject_GetAttrString(other, "_cgrid_internal_key_y");
	pZ = PyObject_GetAttrString(other, "_cgrid_internal_key_z");
	if (!pX || !pY || !pZ)
	{
		PyErr_SetString(PyExc_ValueError, "supplied argument not found in grid (missing _cgrid_internal_key attributes)");
		return NULL;
	}
	k.x = PyInt_AsLong(pX);
	k.y = PyInt_AsLong(pY);
	k.z = PyInt_AsLong(pZ);
	
	pNode = RBExactQuery(self->pTree, &k);
	if (!pNode)
	{
		PyErr_SetString(PyExc_ValueError, "supplied argument not found in grid (no such cell)");
		return NULL;
	}
	pV = (CgridInfo*)pNode->info;
	i = obarr_find(pV->pContents, other);
	if (i == -1)
	{
		PyErr_SetString(PyExc_ValueError, "supplied argument not found in grid (not in cell)");
		return NULL;
	}
	
	element = obarr_get_element(pV->pContents, i);
	
	PyObject_DelAttrString(element, "_cgrid_internal_key_x");
	PyObject_DelAttrString(element, "_cgrid_internal_key_y");
	PyObject_DelAttrString(element, "_cgrid_internal_key_z");
	
	obarr_del_index(pV->pContents, i);
	self->nSize--;
	self->bUnrollDirty = 1;
		
	if (pV->pContents->nSize == 0)
	{
		RBDelete(self->pTree, pNode);
		self->nCells--;
	}
	Py_INCREF(Py_None);
	return Py_None;

}

PyObject* Cgrid_get_radius(PyObject *self_in, PyObject *args)
{
	CgridObject *self = (CgridObject*)self_in;
	PyObject *other = NULL;
	double dRadius;
		
    if (!PyArg_ParseTuple(args, "Od", &other, &dRadius))
	{
		PyErr_SetString(PyExc_TypeError, "wrong arguments");
		return NULL;
	}
	
	return cgrid_get_radius(self, other, dRadius);	
}
	



PySequenceMethods Cgrid_as_seq[] = {
	Cgrid_len,			/* sq_length */
	0,					/* sq_concat */
	0,					/* sq_repeat */
	Cgrid_item,			/* sq_item */
	0,					/* sq_slice */
	0,					/* sq_ass_item */
	0,					/* sq_ass_slice */
	Cgrid_contains,		/* sq_contains */
};

PyMethodDef Cgrid_methods[] = {
	{"insert", (PyCFunction)Cgrid_insert, METH_VARARGS, "allocate the array to a new size"},
	{"delete", (PyCFunction)Cgrid_delete, METH_VARARGS, "remove a grid cell"},
	{"remove", (PyCFunction)Cgrid_remove, METH_VARARGS, "remove an object from its grid cell"},
	{NULL}
};

struct PyMemberDef Cgrid_members[] = {
	/*{"x", T_OBJECT_EX, offsetof(CgridObject, x), 0, "x"},
	{"y", T_OBJECT_EX, offsetof(CgridObject, y), 0, "y"},
	{"z", T_OBJECT_EX, offsetof(CgridObject, z), 0, "z"},*/
	{NULL}  /* Sentinel */
};


PyTypeObject CgridObjectType = {
	PyObject_HEAD_INIT(NULL)
	0,				/* ob_size        */
	"py3dutil.cgrid",		/* tp_name        */
	sizeof(CgridObject),		/* tp_basicsize   */
	0,				/* tp_itemsize    */
	Cgrid_dealloc,	/* tp_dealloc     */
	0,				/* tp_print       */
	0,				/* tp_getattr     */
	0,				/* tp_setattr     */
	0,				/* tp_compare     */
	Cgrid_repr,	    /* tp_repr        */
	0,	/* tp_as_number   */
	Cgrid_as_seq,    /* tp_as_sequence */
	0,				/* tp_as_mapping  */
	0,				/* tp_hash        */
	0,				/* tp_call        */
	0,				/* tp_str         */
	0,				/* tp_getattro    */
	0,				/* tp_setattro    */
	0,				/* tp_as_buffer   */
	Py_TPFLAGS_DEFAULT|Py_TPFLAGS_CHECKTYPES,		/* tp_flags       */
	"Vector objects are simple.",	/* tp_doc         */
	0,				/* tp_traverse       */
	0,				/* tp_clear          */
	0,				/* tp_richcompare    */
	0,				/* tp_weaklistoffset */
	0,				/* tp_iter           */
	0,				/* tp_iternext       */
	Cgrid_methods,   /* tp_methods        */
	Cgrid_members,   /* tp_members        */
	0,    /* tp_getset         */
	0,				/* tp_base           */
	0,				/* tp_dict           */
	0,				/* tp_descr_get      */
	0,				/* tp_descr_set      */
	0,				/* tp_dictoffset     */
	(initproc)Cgrid_init,		/* tp_init           */
};