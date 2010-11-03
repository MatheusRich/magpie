package com.stuffwithstuff.magpie.interpreter.builtin;

import com.stuffwithstuff.magpie.interpreter.Interpreter;
import com.stuffwithstuff.magpie.interpreter.Obj;

public class StringBuiltIns {
  @Signature("call(index Int -> String)")
  public static Obj at(Interpreter interpreter, Obj thisObj, Obj arg) {
    int index = arg.asInt();
    String c = thisObj.asString().substring(index, index + 1);
    return interpreter.createString(c);
  }

  // TODO(bob): May want to strongly-type arg at some point.
  @Signature("compareTo(other -> Int)")
  public static Obj compareTo(Interpreter interpreter, Obj thisObj, Obj arg) {
    return interpreter.createInt(thisObj.asString().compareTo(arg.asString()));
  }

  // TODO(bob): May want to strongly-type arg at some point.
  @Signature("concatenate(other -> String)")
  public static Obj concatenate(Interpreter interpreter, Obj thisObj, Obj arg) {
    String left = thisObj.asString();
    String right = arg.asString();
    
    return interpreter.createString(left + right);
  }

  @Getter("count(-> Int)")
  public static Obj count(Interpreter interpreter, Obj thisObj, Obj arg) {
    return interpreter.createInt(thisObj.asString().length());
  }

  @Signature("substring(arg -> String)")
  public static Obj substring(Interpreter interpreter, Obj thisObj, Obj arg) {
    // TODO(bob): Hackish way to see if we have one or two arguments to this.
    if (arg.getTupleField(0) != null) {
      int startIndex = arg.getTupleField(0).asInt();
      int endIndex = arg.getTupleField(1).asInt();
      String substring = thisObj.asString().substring(startIndex, endIndex);
      return interpreter.createString(substring);
    } else {
      int startIndex = arg.asInt();
      String substring = thisObj.asString().substring(startIndex);
      return interpreter.createString(substring);
    }
  }
}
