#include "ASTExpression.h"

ASTExpression *ASTSequenceExpression::OneStepSimplified() const
{
    if (entries.size() == 1)
        return entries[0]->OneStepSimplified();

    // if we contain matching binary/sequence expressions, or
    // if we have more than one value, then we can do some
    // straightforward optimizations.
    bool canSubsume = false;
    int valueCount = 0;
    bool identity = false;
    for (unsigned int i=0; i<entries.size(); ++i)
    {
        ASTExpression *t = entries[i];
        ASTValue *tv = dynamic_cast<ASTValue*>(t);
        ASTBinaryExpression *tb = dynamic_cast<ASTBinaryExpression*>(t);
        ASTSequenceExpression *ts = dynamic_cast<ASTSequenceExpression*>(t);
        if (tv)
        {
            ++valueCount;
            if ((tv->GetValue() == 1 && op == "*") ||
                (tv->GetValue() == 0 && op == "+"))
                identity = true;
        }
        else if (tb && tb->op == op)
            canSubsume = true;
        else if (ts && ts->op == op)
            canSubsume = true;
    }

    // straightforward optimizations            
    if (canSubsume || identity || valueCount > 1)
    {
        ASTSequenceExpression *expr = new ASTSequenceExpression(op);
        double value;
        bool foundvalue = false;
        for (unsigned int i=0; i<entries.size(); ++i)
        {
            ASTExpression *t = entries[i];
            ASTValue *tv = dynamic_cast<ASTValue*>(t);
            ASTBinaryExpression *tb = dynamic_cast<ASTBinaryExpression*>(t);
            ASTSequenceExpression *ts = dynamic_cast<ASTSequenceExpression*>(t);
            if (tv)
            {
                // if we found a value, combine it into our combined value
                double v = tv->GetValue();
                if (!foundvalue)
                {
                    foundvalue = true;
                    value = v;
                }
                else
                {
                    if (op == "*")
                        value *= v;
                    else if (op == "+")
                        value += v;
                    else
                        THROW(GrammaticalError, "Unknown operator : ", op);
                }
            }
            else if (tb && tb->op == op)
            {
                // can merge in a matching binary expression
                expr->entries.push_back(tb->lhs->OneStepSimplified());
                expr->entries.push_back(tb->rhs->OneStepSimplified());
            }
            else if (ts && ts->op == op)
            {
                // can merge in a matching sequence expression
                for (unsigned int i=0; i<ts->entries.size(); ++i)
                    expr->entries.push_back(ts->entries[i]->OneStepSimplified());
            }
            else 
            {
                expr->entries.push_back(t->OneStepSimplified());
            }
        }
        if (foundvalue)
        {
            if (op == "*" && value==1)
                ; // identity
            else if (op == "+" && value==0)
                ; // identity
            else
                expr->entries.push_back(new ASTReal(value));
        }

        return expr;
    }

    // no more straightforward simplifications

    // factor out the first identifier we see from terms
    if (op == "+" && entries.size() > 2)
    {
        string id = "";
        for (unsigned int i=0; i<entries.size(); ++i)
        {
            ASTExpression *t = entries[i];
            ASTIdentifier *ti = dynamic_cast<ASTIdentifier*>(t);
            ASTBinaryExpression *tb = dynamic_cast<ASTBinaryExpression*>(t);
            ASTSequenceExpression *ts = dynamic_cast<ASTSequenceExpression*>(t);
            if (ti)
            {
                id = ti->name;
                break;
            }
            if (tb && tb->op=="*" && dynamic_cast<ASTIdentifier*>(tb->lhs))
            {
                id = dynamic_cast<ASTIdentifier*>(tb->lhs)->name;
                break;
            }
            if (tb && tb->op=="*" && dynamic_cast<ASTIdentifier*>(tb->rhs))
            {
                id = dynamic_cast<ASTIdentifier*>(tb->rhs)->name;
                break;
            }
            if (ts && ts->op=="*")
            {
                for (unsigned int j=0; j<ts->entries.size(); ++j)
                {
                    if (dynamic_cast<ASTIdentifier*>(ts->entries[j]))
                    {
                        id = dynamic_cast<ASTIdentifier*>(ts->entries[j])->name;
                        break;
                    }
                }
                if (id != "")
                    break;
            }
        }
        if (id != "")
        {
            ASTSequenceExpression *withID = new ASTSequenceExpression(op);
            ASTSequenceExpression *withoutID = new ASTSequenceExpression(op);

            bool hadRawID = false;
            for (unsigned int i=0; i<entries.size(); ++i)
            {
                ASTExpression *t = entries[i];
                ASTIdentifier *ti = dynamic_cast<ASTIdentifier*>(t);
                ASTBinaryExpression *tb = dynamic_cast<ASTBinaryExpression*>(t);
                ASTSequenceExpression *ts = dynamic_cast<ASTSequenceExpression*>(t);
                if (ti && ti->name == id)
                {
                    hadRawID = true;
                    withID->entries.push_back(new ASTReal(1));
                }
                else if (tb && tb->op=="*" &&
                    dynamic_cast<ASTIdentifier*>(tb->lhs) &&
                    dynamic_cast<ASTIdentifier*>(tb->lhs)->name == id)
                {
                    withID->entries.push_back(tb->rhs->OneStepSimplified());
                }
                else if (tb && tb->op=="*" &&
                    dynamic_cast<ASTIdentifier*>(tb->rhs) &&
                    dynamic_cast<ASTIdentifier*>(tb->rhs)->name == id)
                {
                    withID->entries.push_back(tb->lhs->OneStepSimplified());
                }
                else if (ts && ts->op=="*")
                {
                    int index = -1;
                    for (unsigned int j=0; j<ts->entries.size(); ++j)
                    {
                        if (dynamic_cast<ASTIdentifier*>(ts->entries[j]) &&
                            dynamic_cast<ASTIdentifier*>(ts->entries[j])->name == id)
                        {
                            index = j;
                            break;
                        }
                    }
                    if (index != -1)
                    {
                        ASTSequenceExpression *newseq = new ASTSequenceExpression(ts->op);
                        for (int j=0; j<index; ++j)
                            newseq->entries.push_back(ts->entries[j]->OneStepSimplified());
                        for (unsigned int j=index+1; j<ts->entries.size(); ++j)
                            newseq->entries.push_back(ts->entries[j]->OneStepSimplified());
                        withID->entries.push_back(newseq);
                    }
                    else
                    {
                        withoutID->entries.push_back(t->OneStepSimplified());
                    }
                }
                else
                {
                    withoutID->entries.push_back(t->OneStepSimplified());
                }
            }

            ASTExpression *newterm = NULL;
            if (withID->entries.size() == 1)
            {
                if (hadRawID)
                {
                    newterm = new ASTIdentifier(id);
                }
                else
                {
                    newterm = new ASTBinaryExpression("*",
                                                new ASTIdentifier(id),
                                                withID->entries[0]->Cloned());
                }
                delete withID;
            }
            else
            {
                newterm = new ASTBinaryExpression("*",
                                                  new ASTIdentifier(id),
                                                  withID);
            }


            // add the new term into the entries that didn't contain the factor
            if (withoutID->entries.size() == 0)
            {
                delete withoutID;
                return newterm;
            }
            else
            {
                withoutID->entries.push_back(newterm);
                return withoutID;
            }
        }
    }

    // no factorization either, just copy
    ASTSequenceExpression *expr = new ASTSequenceExpression(op);
    for (unsigned int i=0; i<entries.size(); ++i)
        expr->entries.push_back(entries[i]->OneStepSimplified());
    return expr;
}

