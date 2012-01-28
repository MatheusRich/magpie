#include "Fiber.h"

#include "Method.h"
#include "Object.h"
#include "VM.h"

namespace magpie
{
  temp<Fiber> Fiber::create(VM& vm)
  {
    return Memory::makeTemp(new Fiber(vm));
  }
  
  Fiber::Fiber(VM& vm)
  : vm_(vm),
    stack_(),
    callFrames_()
  {}

  temp<Object> Fiber::interpret(gc<Method> method)
  {
    call(method, 0);
    return run();
  }

  temp<Object> Fiber::run()
  {
    int ip = 0;
    bool running = true;
    while (running)
    {
      CallFrame& frame = callFrames_[-1];
      instruction ins = frame.method->code()[ip];
      OpCode op = GET_OP(ins);
      ip++;

      switch (op)
      {
        case OP_MOVE:
        {
          int from = GET_A(ins);
          int to = GET_B(ins);
          store(frame, to, load(frame, from));
          break;
        }

        case OP_CONSTANT:
        {
          int index = GET_A(ins);
          int reg = GET_B(ins);
          store(frame, reg, frame.method->getConstant(index));
          break;
        }

          /*
        case OP_CALL:
        {
          unsigned char argReg = GET_A(instruction);
          unsigned char methodReg = GET_B(instruction);

          gc<Object> arg = frame.getRegister(argReg);
          gc<Object> methodObj = frame.getRegister(methodReg);
          Multimethod* multimethod = frame.getRegister(methodReg)->asMultimethod();

          gc<Chunk> method = multimethod->select(arg);
          // TODO(bob): Handle method not found.

          // Store the IP back into the callframe so we know where to resume
          // when we return to it.
          frame.setInstruction(ip - 1);
          ip = 0;

          call(method, arg);
          break;
        }
           */
          
        case OP_END:
        {
          gc<Object> result = loadRegisterOrConstant(frame, GET_A(ins));
          callFrames_.remove(-1);

          if (callFrames_.count() > 0)
          {
            // Give the result back and resume the calling method.
            CallFrame& caller = callFrames_[-1];
            ip = caller.ip;

            instruction callInstruction = caller.method->code()[ip - 1];
            ASSERT(GET_OP(callInstruction) == OP_CALL,
                   "Should be returning to a call.");
            
            store(caller, GET_C(callInstruction), result);
          }
          else
          {
            // The last method has returned, so end the fiber.
            running = false;
            return result.toTemp();
          }
          break;
        }
          
        case OP_ADD:
        {
          gc<Object> a = loadRegisterOrConstant(frame, GET_A(ins));
          gc<Object> b = loadRegisterOrConstant(frame, GET_B(ins));
          
          // TODO(bob): Handle non-number types.
          double c = a->toNumber() + b->toNumber();
          temp<Object> num = Object::create(c);
          store(frame, GET_C(ins), num);
          break;
        }
          
        case OP_SUBTRACT:
        {
          gc<Object> a = loadRegisterOrConstant(frame, GET_A(ins));
          gc<Object> b = loadRegisterOrConstant(frame, GET_B(ins));
          
          // TODO(bob): Handle non-number types.
          double c = a->toNumber() - b->toNumber();
          temp<Object> num = Object::create(c);
          store(frame, GET_C(ins), num);
          break;
        }
          
        case OP_MULTIPLY:
        {
          gc<Object> a = loadRegisterOrConstant(frame, GET_A(ins));
          gc<Object> b = loadRegisterOrConstant(frame, GET_B(ins));
          
          // TODO(bob): Handle non-number types.
          double c = a->toNumber() * b->toNumber();
          temp<Object> num = Object::create(c);
          store(frame, GET_C(ins), num);
          break;
        }
          
        case OP_DIVIDE:
        {
          gc<Object> a = loadRegisterOrConstant(frame, GET_A(ins));
          gc<Object> b = loadRegisterOrConstant(frame, GET_B(ins));
          
          // TODO(bob): Handle non-number types.
          double c = a->toNumber() / b->toNumber();
          temp<Object> num = Object::create(c);
          store(frame, GET_C(ins), num);
          break;
        }
          
        default:
          ASSERT(false, "Unknown opcode.");
          break;
      }
    }
    
    ASSERT(false, "Should not get here.");
    return temp<Object>();
  }

  void Fiber::call(gc<Method> method, int stackStart)
  {
    // Allocate registers for the method.
    // TODO(bob): Make this a single operation on Array.
    while (stack_.count() < stackStart + method->numRegisters())
    {
      stack_.add(gc<Object>());
    }
    
    callFrames_.add(CallFrame(method, stackStart));
  }

  
  gc<Object> Fiber::loadRegisterOrConstant(const CallFrame& frame, int index)
  {
    if (IS_CONSTANT(index))
    {
      return frame.method->getConstant(GET_CONSTANT(index));
    }
    else
    {
      return load(frame, index);
    }
  }
}