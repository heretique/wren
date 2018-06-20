#include "wren_opt_meta.h"

#if WREN_OPT_META

#include <string.h>

#include "wren_vm.h"
#include "wren_opt_meta.wren.inc"

static void metaCompile(WrenVM* vm)
{
  const char* source = wrenGetSlotString(vm, 1);
  bool isExpression = wrenGetSlotBool(vm, 2);
  bool printErrors = wrenGetSlotBool(vm, 3);

  // TODO: Allow passing in module?
  ObjClosure* closure = wrenCompileSource(vm, "main", source,
                                          isExpression, printErrors);

  // Return the result. We can't use the public API for this since we have a
  // bare ObjClosure*.
  if (closure == NULL)
  {
    vm->apiStack[0] = NULL_VAL;
  }
  else
  {
    vm->apiStack[0] = OBJ_VAL(closure);
  }
}

static void metaGetModuleVariables(WrenVM* vm)
{
  wrenEnsureSlots(vm, 3);
  
  Value moduleValue = wrenMapGet(vm->modules, vm->apiStack[1]);
  if (IS_UNDEFINED(moduleValue))
  {
    vm->apiStack[0] = NULL_VAL;
    return;
  }
    
  ObjModule* module = AS_MODULE(moduleValue);
  ObjList* names = wrenNewList(vm, module->variableNames.count);
  vm->apiStack[0] = OBJ_VAL(names);

  // Initialize the elements to null in case a collection happens when we
  // allocate the strings below.
  for (int i = 0; i < names->elements.count; i++)
  {
    names->elements.data[i] = NULL_VAL;
  }
  
  for (int i = 0; i < names->elements.count; i++)
  {
    names->elements.data[i] = OBJ_VAL(module->variableNames.data[i]);
  }
}

static void classMirrorDefinesMethod(WrenVM* vm)
{
  Value classValue = vm->apiStack[1];
  Value methodValue = vm->apiStack[2];

  if (!IS_CLASS(classValue) || !IS_STRING(methodValue))
  {
    wrenSetSlotNull(vm, 0);
    return;
  }

  const ObjClass* classObj = AS_CLASS(classValue);
  const ObjString *methodStr = AS_STRING(methodValue);

  wrenSetSlotBool(vm, 0, wrenFindMethod(vm, classObj, methodStr) != NULL);
}

static void classMirrorDefinesClass(WrenVM* vm)
{
  Value classValue = vm->apiStack[1];
  Value anotherClassValue = vm->apiStack[2];

  if (!IS_CLASS(classValue) || !IS_CLASS(anotherClassValue))
  {
    wrenSetSlotNull(vm, 0);
    return;
  }

  ObjClass* classObj = AS_CLASS(classValue);
  ObjClass* anotherClassObj = AS_CLASS(anotherClassValue);

  if (classObj->methods.count < anotherClassObj->methods.count)
  {
    wrenSetSlotBool(vm, 0, false);
    return;
  }

  for (int symbol = 0; symbol < anotherClassObj->methods.count; ++symbol)
  {
    if (classObj->methods.data[symbol].type == METHOD_NONE &&
        anotherClassObj->methods.data[symbol].type != METHOD_NONE)
    {
      wrenSetSlotBool(vm, 0, false);
      return;
    }
  }

  wrenSetSlotBool(vm, 0, true);
}

const char* wrenMetaSource()
{
  return metaModuleSource;
}

WrenForeignMethodFn wrenMetaBindForeignMethod(WrenVM* vm,
                                              const char* className,
                                              bool isStatic,
                                              const char* signature)
{
  // There is only one foreign method in the meta module.
  ASSERT(isStatic, "Should be static.");
  
  if (strcmp(className, "Meta") == 0)
  {
    if (strcmp(signature, "compile_(_,_,_)") == 0)
    {
      return metaCompile;
    }
    if (strcmp(signature, "getModuleVariables_(_)") == 0)
    {
      return metaGetModuleVariables;
    }
  }
  else if (strcmp(className, "ClassMirror") == 0)
  {
    if (strcmp(signature, "definesMethod(_,_)") == 0)
    {
      return classMirrorDefinesMethod;
    }
    if (strcmp(signature, "definesClass(_,_)") == 0)
    {
      return classMirrorDefinesClass;
    }
  }
  
  ASSERT(false, "Unknown method.");
  return NULL;
}

#endif
