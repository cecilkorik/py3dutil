#include "obarr.h"
#include "cgrid.h"


static PyMethodDef ModMethods[] = {
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

PyMODINIT_FUNC
initpy3dutil(void)
{
	PyObject* m;

	ObarrObjectType.tp_new = PyType_GenericNew;
	CgridObjectType.tp_new = PyType_GenericNew;
	if (PyType_Ready(&ObarrObjectType) < 0)
		return;
	if (PyType_Ready(&CgridObjectType) < 0)
		return;

    (void) Py_InitModule("py3dutil", ModMethods);
	m = Py_InitModule3("py3dutil", NULL,
			   "Example module that creates an extension type.");
	if (m == NULL)
		return;

	Py_INCREF(&ObarrObjectType);
	Py_INCREF(&CgridObjectType);
	PyModule_AddObject(m, "obarr", (PyObject *)&ObarrObjectType);
	PyModule_AddObject(m, "cgrid", (PyObject *)&CgridObjectType);
}
