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
