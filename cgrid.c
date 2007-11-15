#include "cgrid.h"
#include "obarr.h"
#include "red_black_tree.h"

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


int Cgrid_init(CgridObject *self, PyObject *args, PyObject *kwds)
{
	double fCell;
	
    if (!PyArg_ParseTuple(args, "d", &fCell))
	{
		PyErr_SetString(PyExc_TypeError, "wrong arguments");
		return 1;
	}
	
	self->pTree = RBTreeCreate(cgrid_compare, cgrid_destroykey, cgrid_destroyinfo, cgrid_printkey, cgrid_printinfo);
	self->nSize = 0;
	self->nCells = 0;
	self->fCellSize = fCell;
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
