#include "obarr.h"
/*#include "cgrid.h"*/
#include "vect.h"
#include "quat.h"


static PyMethodDef ModMethods[] = {
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

PyMODINIT_FUNC
initpy3dutil(void)
{
	PyObject* m;

	ObarrObjectType.tp_new = PyType_GenericNew;
	if (PyType_Ready(&ObarrObjectType) < 0)
		return;
	/*CgridObjectType.tp_new = PyType_GenericNew;
	if (PyType_Ready(&CgridObjectType) < 0)
		return;*/
	VectObjectType.tp_new = PyType_GenericNew;
	if (PyType_Ready(&VectObjectType) < 0)
		return;
	QuatObjectType.tp_new = PyType_GenericNew;
	if (PyType_Ready(&QuatObjectType) < 0)
		return;

    (void) Py_InitModule("py3dutil", ModMethods);
	m = Py_InitModule3("py3dutil", NULL,
			   "Various 3d utilities to accelerate 3d games");
	if (m == NULL)
		return;

	Py_INCREF(&ObarrObjectType);
	PyModule_AddObject(m, "obarr", (PyObject *)&ObarrObjectType);
	/*Py_INCREF(&CgridObjectType);
	PyModule_AddObject(m, "cgrid", (PyObject *)&CgridObjectType);*/
	Py_INCREF(&VectObjectType);
	PyModule_AddObject(m, "vect", (PyObject *)&VectObjectType);
	Py_INCREF(&QuatObjectType);
	PyModule_AddObject(m, "quat", (PyObject *)&QuatObjectType);
}
