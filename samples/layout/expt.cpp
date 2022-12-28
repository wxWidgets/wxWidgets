/*
 * Experimental code to use operators for constraint specification.
 * In the end, the syntax didn't look much clearer than
 * the original, so abandoned.

 Example:

  wxConstrain(frame->panel,

    ( leftOf   (frame->panel) = leftOf   (frame),
      topOf    (frame->panel) = topOf    (frame),
      rightOf  (frame->panel) = rightOf  (frame),
      heightOf (frame->panel) = 50 % heightOf (frame)
    )
  );
 */

// Operator experiments
#define wxCONSTRAINT_OP_REL   1
#define wxCONSTRAINT_OP_PERCENT 2
#define wxCONSTRAINT_OP_EDGE  3
#define wxCONSTRAINT_OP_ABS   4
#define wxCONSTRAINT_OP_AND   5

class wxConstraintOp: public wxObject
{
 public:
   int opType;
   wxEdge edge;
   wxRelationship relationship;
   wxWindow *win;
   int value;
   int margin;
   wxConstraintOp *lhs;
   wxConstraintOp *rhs;
   wxConstraintOp(int typ)
   {
     opType = typ;
     edge = wxLeft;
     win = nullptr;
     value = 0;
     margin = 0;
     relationship = wxSameAs;
     lhs = 0; rhs = 0;
   }
   wxConstraintOp(const wxConstraintOp& op)
   {
     opType = op.opType;
     edge = op.edge;
     relationship = op.relationship;
     win = op.win;
     value = op.value;
     margin = op.margin;
     if (op.lhs)
       lhs = new wxConstraintOp(*op.lhs);
     else
       lhs = nullptr;
     if (op.rhs)
       rhs = new wxConstraintOp(*op.rhs);
     else
       rhs = nullptr;
   }
   ~wxConstraintOp(void)
   {
     if (lhs)
       delete lhs;
     if (rhs)
       delete rhs;
   }

  wxConstraintOp operator = (const wxConstraintOp& arg2);
  wxConstraintOp operator = (const int value);

  friend wxConstraintOp operator % (const int perCent, const wxConstraintOp& arg2);
  friend wxConstraintOp operator + (wxConstraintOp& arg1, int margin);
  friend wxConstraintOp operator - (wxConstraintOp& arg1, int margin);
  friend wxConstraintOp operator , (const wxConstraintOp& arg1, const wxConstraintOp& arg2);
};

wxConstraintOp leftOf(wxWindow *win)
{
  wxConstraintOp thing(wxCONSTRAINT_OP_EDGE);
  thing.win = win;
  thing.edge = wxLeft;
  return thing;
}

wxConstraintOp topOf(wxWindow *win)
{
  wxConstraintOp thing(wxCONSTRAINT_OP_EDGE);
  thing.win = win;
  thing.edge = wxTop;
  return thing;
}

wxConstraintOp widthOf(wxWindow *win)
{
  wxConstraintOp thing(wxCONSTRAINT_OP_EDGE);
  thing.win = win;
  thing.edge = wxWidth;
  return thing;
}

wxConstraintOp wxConstraintOp::operator = (const wxConstraintOp& arg2)
{
  wxConstraintOp op3(wxCONSTRAINT_OP_REL);
  op3.relationship = wxPercentOf;
  op3.value = 100;
  if ((op3.relationship == wxPercentOf) && (op3.value > 0))
    op3.value = this->value;
  op3.lhs = new wxConstraintOp(*this);
  op3.rhs = new wxConstraintOp(arg2);

  return op3;
}

wxConstraintOp wxConstraintOp::operator = (const int value)
{
  wxConstraintOp op3(wxCONSTRAINT_OP_REL);
  op3.relationship = wxAbsolute;

  op3.lhs = new wxConstraintOp(*this);
  op3.rhs = new wxConstraintOp(wxCONSTRAINT_OP_ABS);
  op3.value = value;

  return op3;
}

wxConstraintOp operator % (const int perCent, const wxConstraintOp& arg2)
{
  wxConstraintOp op3(arg2);
  op3.opType = wxCONSTRAINT_OP_EDGE;
  op3.value = perCent;
  if (op3.value > 0)
    op3.value = arg2.value;

  return op3;
}

wxConstraintOp operator + (wxConstraintOp& arg1, int margin)
{
  wxConstraintOp op3(arg1);
  op3.margin = margin;
  return op3;
}

wxConstraintOp operator - (wxConstraintOp& arg1, int margin)
{
  wxConstraintOp op3(arg1);
  op3.margin = - margin;
  return op3;
}

wxConstraintOp operator , (const wxConstraintOp& arg1, const wxConstraintOp& arg2)
{
  wxConstraintOp op3(wxCONSTRAINT_OP_AND);
  op3.lhs = new wxConstraintOp(arg1);
  op3.rhs = new wxConstraintOp(arg2);

  return op3;
}
