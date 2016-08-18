
import "meta" for ClassMirror

class Foo {
  foo(arg) { }
}

class Bar {
  bar { }
  foo(arg) { }
}

System.print(ClassMirror.definesMethod(Foo, "bar")) // expect: false
System.print(ClassMirror.definesMethod(Foo, "foo(_)")) // expect: true
System.print(ClassMirror.definesClass(Foo, Bar)) // expect: false
System.print(ClassMirror.definesClass(Bar, Foo)) // expect: true
