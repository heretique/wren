class Meta {
  static getModuleVariables(module) {
    if (!(module is String)) Fiber.abort("Module name must be a string.")
    var result = getModuleVariables_(module)
    if (result != null) return result

    Fiber.abort("Could not find a module named '%(module)'.")
  }

  static eval(source) {
    if (!(source is String)) Fiber.abort("Source code must be a string.")

    var fiber = compile_(source, false, false)
    // TODO: Include compile errors.
    if (fiber == null) Fiber.abort("Could not compile source code.")

    fiber.call()
  }

  static compileExpression(source) {
    if (!(source is String)) Fiber.abort("Source code must be a string.")
    return compile_(source, true, true)
  }

  static compile(source) {
    if (!(source is String)) Fiber.abort("Source code must be a string.")
    return compile_(source, false, true)
  }

  foreign static compile_(source, isExpression, printErrors)
  foreign static getModuleVariables_(module)
}

class ClassMirror {
  foreign static definesMethod(aClass, method)
  foreign static definesClass(aClass, anotherClass)
}
