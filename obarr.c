#define NEED_STATIC
#include "obarr.h"
#undef NEED_STATIC

PyObject* obarr_get_element(ObarrObject* self, long index)
{
	return self->pData[index];	
}

int obarr_find(ObarrObject* self, PyObject* other_in)
{
	long i;
	for (i = 0; i < self->nSize; i++)
	{
		if (self->pData[i] == other_in)
			return i;
		
	}
	return -1;
}


void obarr_set_element(ObarrObject* self, long index, PyObject* new_in)
{
	PyObject* old = obarr_get_element(self, index);
	Py_INCREF(new_in);
	self->pData[index] = new_in;
	Py_DECREF(old);
}

void obarr_empty(ObarrObject* self)
{
	long i;
	for (i = 0; i < self->nAllocSize; i++)
	{
		Py_DECREF(self->pData[i]);
	}
	if (self->pData != NULL)
		free(self->pData);
	self->nAllocSize = 0;
	self->nSize = 0;
	self->pData = NULL;
}

void obarr_del_index(ObarrObject* self, long i)
{
	Py_DECREF(self->pData[i]);
	
	if ((self->nSize - i - 1) > 0)
	{	
		/* slide the tail end of the array up so we don't have a hole in the middle */
		self->pData[i] = self->pData[self->nSize - 1];
		
		/* fill the end of the array with a None before we reduce the size to cover that up */
		/* to turn it back into "allocated but unused" space */
		self->pData[self->nSize - 1] = Py_None;
		Py_INCREF(Py_None);
	}
	else
	{
		self->pData[i] = Py_None;
		Py_INCREF(Py_None);
	}
	self->nSize = self->nSize - 1;
}

int obarr_valid_index(ObarrObject* self, long i)
{
	if (i >= 0 && i < self->nSize)
		return 1;
	return 0;
}

int obarr_set_size(ObarrObject* self, long size)
{
	long i, newsize;
	void* tmp;
	
	if (size == 0)
	{
		obarr_empty(self);
		return 1;
	}
	else if (size > self->nAllocSize)
	{
		newsize = self->nChunkSize * (size / self->nChunkSize);
		if ((size % self->nChunkSize) != 0)
			newsize = newsize + self->nChunkSize;
		if (self->pData == NULL)
		{
			self->pData = (PyObject**)malloc(newsize * sizeof(void*));
			if (self->pData == NULL)
			{
				return 0;
			}
			for (i = 0; i < newsize; i++)
			{
				Py_INCREF(Py_None);
				self->pData[i] = Py_None;
			}
		}
		else
		{
			tmp = realloc(self->pData, newsize * sizeof(void*));
			if (tmp == NULL)
				return 0;
			self->pData = (PyObject**)tmp;
			for (i = self->nAllocSize; i < newsize; i++)
			{
				Py_INCREF(Py_None);
				self->pData[i] = Py_None;
			}
		}
		self->nAllocSize = newsize;
	}
	else
	{
		for (i = size; i < self->nAllocSize; i++)
		{
			if (self->pData[i] != Py_None)
			{
				Py_DECREF(self->pData[i]);
				Py_INCREF(Py_None);
				self->pData[i] = Py_None;
			}
		}
	}
	self->nSize = size;
	return 1;
}

int obarr_append(ObarrObject *self, PyObject *other)
{
	if (obarr_set_size(self, self->nSize + 1))
	{
		obarr_set_element(self, self->nSize - 1, other);
		return 1;
	}
	return 0;
}

int Obarr_init(ObarrObject *self, PyObject *args, PyObject *kwds)
{
	self->nSize = 0;
	self->nChunkSize = 64;
	self->nAllocSize = 0;
	self->pData = NULL;

	return 0;
}

void Obarr_dealloc(PyObject* self_in)
{
	ObarrObject* self = (ObarrObject*)self_in;
	obarr_empty(self);
}

PyObject* Obarr_repr(PyObject *self_in)
{
	ObarrObject *self;
	PyObject *tuple, *fmtstring, *reprstring;
	if (!Obarr_Check(self_in))
		return PyString_FromString("<unknown object type>");
	
	self = (ObarrObject*)self_in;
	tuple = Py_BuildValue("(l)", self->nSize);
	fmtstring = PyString_FromString("<array of %d objects>");
	reprstring = PyString_Format(fmtstring, tuple);
	Py_DECREF(tuple);
	Py_DECREF(fmtstring);
	return reprstring;
}

Py_ssize_t Obarr_len(PyObject *self_in)
{
	ObarrObject* self = (ObarrObject*)self_in;
	return self->nSize;
}


PyObject * Obarr_item(PyObject *self_in, Py_ssize_t index)
{
	ObarrObject* self = (ObarrObject*)self_in;
	PyObject* ob;
	if (!obarr_valid_index(self, index))
	{
		PyErr_SetString(PyExc_IndexError, "index not in range");
		return NULL;
	}
	
	ob = obarr_get_element(self, index);
	Py_INCREF(ob);
	return ob;

}

int Obarr_setitem(PyObject* self_in, Py_ssize_t index, PyObject* new_in)
{
	ObarrObject* self = (ObarrObject*)self_in;
	if (!obarr_valid_index(self, index))
	{
		PyErr_SetString(PyExc_IndexError, "index not in range");
		return -1;
	}
	obarr_set_element(self, index, new_in);
	
	return 0;
}

PyObject* Obarr_remove(PyObject* self_in, PyObject* args)
{
	ObarrObject* self = (ObarrObject*)self_in;
	long i;
	PyObject* other_in = NULL;
    if (!PyArg_ParseTuple(args, "O", &other_in))
	{
		PyErr_SetString(PyExc_TypeError, "wrong arguments");
		return NULL;
	}
	i = obarr_find(self, other_in);
	if (i == -1)
	{
		PyErr_SetString(PyExc_ValueError, "supplied argument not found in list");
		return NULL;
	}
	obarr_del_index(self, i);
	
	Py_INCREF(Py_None);
	return Py_None;
		
}

PyObject* Obarr_delete(PyObject* self_in, PyObject* args)
{
	ObarrObject* self = (ObarrObject*)self_in;
	long i;
    if (!PyArg_ParseTuple(args, "l", &i))
	{
		PyErr_SetString(PyExc_TypeError, "must be called with an index as an argument");
		return NULL;
	}
	if (!obarr_valid_index(self, i))
	{
		PyErr_SetString(PyExc_IndexError, "index not in range");
		return NULL;
	}
	
	obarr_del_index(self, i);
	
	Py_INCREF(Py_None);
	return Py_None;
}

int Obarr_contains(PyObject* self_in, PyObject* other_in)
{
	ObarrObject* self = (ObarrObject*)self_in;
	long i;
	i = obarr_find(self, other_in);
	if (i == -1)
		return 0;
	return 1;
}

PyObject* Obarr_index(PyObject* self_in, PyObject* args)
{
	ObarrObject* self = (ObarrObject*)self_in;
	long i;
	PyObject* other_in = NULL;
    if (!PyArg_ParseTuple(args, "O", &other_in))
	{
		PyErr_SetString(PyExc_TypeError, "wrong arguments");
		return NULL;
	}
	i = obarr_find(self, other_in);
	if (i == -1)
	{
		PyErr_SetString(PyExc_ValueError, "not found");
		return NULL;
	}
	return PyInt_FromLong(i);
}

PyObject* Obarr_find(PyObject* self_in, PyObject* args)
{
	ObarrObject* self = (ObarrObject*)self_in;
	long i;
	PyObject* other_in = NULL;
	if (!PyArg_ParseTuple(args, "O", &other_in))
	{
		PyErr_SetString(PyExc_TypeError, "wrong arguments");
		return NULL;
	}

	i = obarr_find(self, other_in);
	return PyInt_FromLong(i);
}

int compare_doubles(const void *a, const void *b)
{
	const sortkey *sa = (const sortkey*)a;
	const sortkey *sb = (const sortkey*)b;
	
	return (sa->d > sb->d) - (sa->d < sb->d);
}

PyObject* Obarr_sort(PyObject* self_in, PyObject* args)
{
	ObarrObject* self = (ObarrObject*)self_in;
	ObarrObject* other = NULL;
	PyObject* other_in = NULL;
	PyObject* pyfloat = NULL;
	sortkey* pSortkeys = NULL;
	PyObject** pNewData = NULL;
	long i;

	if (!PyArg_ParseTuple(args, "O!", &ObarrObjectType, &other_in))
	{
		PyErr_SetString(PyExc_TypeError, "wrong arguments");
		return NULL;
	}
	other = (ObarrObject*)other_in;
	
	if (other->nSize != self->nSize)
	{
		PyErr_SetString(PyExc_ValueError, "array to sort by must be the same size");
		return NULL;
	}
	
	pSortkeys = malloc(sizeof(sortkey) * self->nSize);
	if (!pSortkeys)
	{
		PyErr_SetString(PyExc_MemoryError, "insufficient free memory");
		return NULL;
	}
	
	for (i = 0; i < self->nSize; i++)
	{
		pyfloat = other->pData[i];
		if (!PyFloat_Check(pyfloat))
		{
			PyErr_SetString(PyExc_TypeError, "array to sort by must contain only floats");
			return NULL;
		}
		pSortkeys[i].d = PyFloat_AsDouble(pyfloat);
		pSortkeys[i].i = i;
	}
	
	qsort(pSortkeys, self->nSize, sizeof(sortkey), compare_doubles);
	
	pNewData = malloc(self->nAllocSize * sizeof(void*));
	if (!pNewData)
	{
		PyErr_SetString(PyExc_MemoryError, "insufficient free memory");
		return NULL;
	}

	memcpy(pNewData, self->pData, self->nAllocSize * sizeof(void*));
	for (i = 0; i < self->nSize; i++)
	{
		pNewData[i] = self->pData[pSortkeys[i].i];
	}
	free(pSortkeys);
	free(self->pData);
	self->pData = pNewData;
	
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject* Obarr_append(PyObject* self_in, PyObject* args)
{
	ObarrObject* self = (ObarrObject*)self_in;
	PyObject* other_in = NULL;
	long index;
	int r;
	if (!PyArg_ParseTuple(args, "O", &other_in))
	{
		PyErr_SetString(PyExc_TypeError, "wrong arguments");
		return NULL;
	}
	index = self->nSize;
	r = obarr_set_size(self, self->nSize + 1);
	if (!r)
	{
		PyErr_SetString(PyExc_MemoryError, "insufficient free memory");
		return NULL;
	}
	
	obarr_set_element(self, index, other_in);
	Py_INCREF(Py_None);
	return Py_None;
	
}

PyObject* Obarr_resize(PyObject* self_in, PyObject* args)
{
	ObarrObject* self = (ObarrObject*)self_in;
	long newsize;
	int r;
    if (!PyArg_ParseTuple(args, "l", &newsize))
	{
		PyErr_SetString(PyExc_TypeError, "wrong arguments");
		return NULL;
	}
	r = obarr_set_size(self, newsize);
	if (!r)
	{
		PyErr_SetString(PyExc_MemoryError, "insufficient free memory");
		return NULL;
	}
	Py_INCREF(Py_None);
	return Py_None;
}
	


PyObject* Obarr_clear(PyObject* self_in, PyObject* args)
{
	ObarrObject* self = (ObarrObject*)self_in;
	obarr_empty(self);
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject* Obarr_debug(PyObject* self_in, PyObject* args)
{
	ObarrObject* self = (ObarrObject*)self_in;
	printf("AllocSize: %d\n", self->nAllocSize);
	printf("Chunk: %d\n", self->nChunkSize);
	printf("Size: %d\n", self->nSize);
	Py_INCREF(Py_None);
	return Py_None;
	
}

/* Python object definition structures */
PySequenceMethods Obarr_as_seq[] = {
	Obarr_len,			/* sq_length */
	0,					/* sq_concat */
	0,					/* sq_repeat */
	Obarr_item,			/* sq_item */
	0,					/* sq_slice */
	Obarr_setitem,		/* sq_ass_item */
	0,					/* sq_ass_slice */
	Obarr_contains,		/* sq_contains */
};

PyMethodDef Obarr_methods[] = {
	{"resize", (PyCFunction)Obarr_resize, METH_VARARGS, "allocate the array to a new size"},
	{"clear", (PyCFunction)Obarr_clear, METH_NOARGS, "delete everything in the array"},
	{"append", (PyCFunction)Obarr_append, METH_VARARGS, "append the object to the array"},
	{"find", (PyCFunction)Obarr_find, METH_VARARGS, "find the index of an object in the array"},
	{"index", (PyCFunction)Obarr_index, METH_VARARGS, "find the index of an object in the array"},
	{"debug", (PyCFunction)Obarr_debug, METH_NOARGS, "debug"},
	{"delete", (PyCFunction)Obarr_delete, METH_VARARGS, "delete a specific index in the array"},
	{"remove", (PyCFunction)Obarr_remove, METH_VARARGS, "remove an object from the array"},
	{"sort", (PyCFunction)Obarr_sort, METH_VARARGS, "sorts the array using another obarr of floats as sortkeys"},
	{NULL}
};

struct PyMemberDef Obarr_members[] = {
	/*{"x", T_OBJECT_EX, offsetof(ObarrObject, x), 0, "x"},
	{"y", T_OBJECT_EX, offsetof(ObarrObject, y), 0, "y"},
	{"z", T_OBJECT_EX, offsetof(ObarrObject, z), 0, "z"},*/
	{NULL}  /* Sentinel */
};


PyTypeObject ObarrObjectType = {
	PyObject_HEAD_INIT(NULL)
	0,				/* ob_size        */
	"py3dutil.obarr",		/* tp_name        */
	sizeof(ObarrObject),		/* tp_basicsize   */
	0,				/* tp_itemsize    */
	Obarr_dealloc,	/* tp_dealloc     */
	0,				/* tp_print       */
	0,				/* tp_getattr     */
	0,				/* tp_setattr     */
	0,				/* tp_compare     */
	Obarr_repr,	    /* tp_repr        */
	0,	/* tp_as_number   */
	Obarr_as_seq,    /* tp_as_sequence */
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
	Obarr_methods,   /* tp_methods        */
	Obarr_members,   /* tp_members        */
	0,    /* tp_getset         */
	0,				/* tp_base           */
	0,				/* tp_dict           */
	0,				/* tp_descr_get      */
	0,				/* tp_descr_set      */
	0,				/* tp_dictoffset     */
	(initproc)Obarr_init,		/* tp_init           */
};


