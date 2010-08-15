package com.stuffwithstuff.magpie.parser;

import java.util.ArrayList;
import java.util.List;

import com.stuffwithstuff.magpie.ast.VariableExpr;
import com.stuffwithstuff.magpie.ast.Expr;
import com.stuffwithstuff.magpie.ast.FnExpr;
import com.stuffwithstuff.magpie.ast.FunctionType;

public class VariableExprParser implements ExprParser {

  @Override
  public Expr parse(MagpieParser parser) {
    Position startPos = parser.consume(TokenType.VAR).getPosition();
    
    // TODO(bob): support multiple definitions and tuple decomposition here
    String name = parser.consume(TokenType.NAME).getString();
    
    // See if we're defining a function in shorthand notation:
    // def foo() blah
    if (parser.lookAhead(TokenType.LEFT_PAREN)) {
      Position fnPosition = parser.last(1).getPosition();
      
      List<String> paramNames = new ArrayList<String>();
      FunctionType type = parser.functionType(paramNames);
      Expr body = parser.parseBlock();
      
      // Desugar it to: def foo = fn () blah
      FnExpr function = new FnExpr(fnPosition.union(body.getPosition()),
          paramNames, type.getParamType(), type.getReturnType(), body);
      return new VariableExpr(startPos.union(function.getPosition()),
          name, function);
    } else {
      // Just a regular variable definition.
      parser.consume(TokenType.EQUALS);
      
      Expr value = parser.parseExpression();
      return new VariableExpr(startPos.union(value.getPosition()),
          name, value);
    }
  }
}
