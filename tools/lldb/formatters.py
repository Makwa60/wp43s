firstTime = True

def real34formatter(valobj, internal_dict, options):
  global firstTime
  frame = valobj.GetFrame()
  value = valobj.GetName()
  # It appears that the first time LLDB comes across real34_t it has problems processing it in
  # certain contexts, so just try again and it works
  if firstTime:
    frame.EvaluateExpression("debuggerReal34((real34_t *)0)")
    firstTime = False
  return frame.EvaluateExpression("debuggerReal34((real34_t *){0})".format(value)).GetSummary()

