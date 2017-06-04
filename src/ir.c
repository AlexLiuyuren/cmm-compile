#include "symbol_table.h"
#include "ir.h"
#include "common.h"

const struct Operand NULLOP;

InterCodeNode InterCodeHead;
int InterCodeVarCnt, InterCodeTempCnt, InterCodeLabelCnt;

void initIR()
{
	InterCodeHead.next = &InterCodeHead;
	InterCodeHead.prev = &InterCodeHead;
	InterCodeVarCnt = 0, InterCodeTempCnt = 0, InterCodeLabelCnt = 0;
}

Operand generateTemp()
{
	Operand result;
	result.kind = TEMP;
	result.var_no = InterCodeTempCnt;
	InterCodeTempCnt++;
	return result;
}

Operand generateLabel()
{
	Operand result;
	result.kind = LABEL;
	result.var_no = InterCodeLabelCnt;
	InterCodeLabelCnt++;
	return result;
}
	
InterCodeNode *InterCodeCat(int cnt, ...)
{
	va_list args;
	va_start(args, cnt);
	InterCodeNode *result = va_arg(args, InterCodeNode *);
	int i = 1;
	for (; i < cnt; i++) {
		InterCodeNode *temp = va_arg(args, InterCodeNode *);
		if(temp->next != temp && temp->prev != temp) {
			result->prev->next = temp->next;
			temp->next->prev = result->prev;
			result->prev = temp->prev;
			temp->prev->next = result;
		}
	}
	va_end(args);
	return result;
}

InterCodeNode *InterCodeAppend(InterCodeNode *head, InterCode code)
{
	if (head == NULL) {
		fprintf(stderr, "InterCodeAppend head is null.\n");
		return head;
	}
	InterCodeNode *p = (InterCodeNode *) malloc(sizeof(InterCodeNode));
	p->code = code;
	if(head->prev == head && head->next == head) {
		head->prev = head->next = p;
		p->prev = p->next = head;
	}
	else {
		head->prev->next = p;
		p->prev = head->prev;
		p->next = head;
		head->prev = p;
	}
	return head;	
}

void deleteInterCode()
{
	while(InterCodeHead.next != &InterCodeHead) {
		InterCodeNode *p = InterCodeHead.next;
		InterCodeHead.next = p->next;
		p->next->prev = &InterCodeHead;
		free(p);
	}
}

void printOperand(Operand *op, char *str)
{
	switch(op->kind) {
	case VARIABLE:
		sprintf(str, "v%d", op->var_no);
		break;
	case TEMP:
		sprintf(str, "t%d", op->var_no);
		break;
	case LABEL:
		sprintf(str, "label%d", op->var_no);
		break;
	case CONSTANT:
		sprintf(str, "#%d", op->value);
		break;
	case ADDRESS:
		assert(0); 
		break;
	default:
		assert(0);
	}
}

void printInterCode(FILE *fp)
{	
	InterCodeNode *p = InterCodeHead.next;
	char resulttemp[kMaxLen], ltemp[kMaxLen], rtemp[kMaxLen];
	while (p != &InterCodeHead) {
		switch(p->code.kind) {
		case ASSIGN:
			printOperand(&p->code.assign.left, ltemp);
			printOperand(&p->code.assign.right, rtemp);
			fprintf(fp, "%s := %s\n", ltemp, rtemp);
			break;
		case ADD:
			printOperand(&p->code.binop.result, resulttemp);
			printOperand(&p->code.binop.op1, ltemp);
			printOperand(&p->code.binop.op2, rtemp);
			fprintf(fp, "%s := %s + %s\n", resulttemp, ltemp, rtemp);
			break;
		case SUB:
			printOperand(&p->code.binop.result, resulttemp);
			printOperand(&p->code.binop.op1, ltemp);
			printOperand(&p->code.binop.op2, rtemp);
			fprintf(fp, "%s := %s - %s\n", resulttemp, ltemp, rtemp);
			break;
		case MUL:
			printOperand(&p->code.binop.result, resulttemp);
			printOperand(&p->code.binop.op1, ltemp);
			printOperand(&p->code.binop.op2, rtemp);
			fprintf(fp, "%s := %s * %s\n", resulttemp, ltemp, rtemp);
			break;
		case DIV_:
			printOperand(&p->code.binop.result, resulttemp);
			printOperand(&p->code.binop.op1, ltemp);
			printOperand(&p->code.binop.op2, rtemp);
			fprintf(fp, "%s := %s / %s\n", resulttemp, ltemp, rtemp);
			break;
		case LABEL_CODE:
			printOperand(&p->code.label_code, resulttemp);
			fprintf(fp, "LABEL %s :\n", resulttemp);
			break;
		case LABEL_GOTO:
			printOperand(&p->code.label_goto, resulttemp);
			fprintf(fp, "GOTO %s\n", resulttemp);
			break;
		case LABEL_COND:
			printOperand(&p->code.label_cond.left, ltemp);
			printOperand(&p->code.label_cond.right, rtemp);
			printOperand(&p->code.label_cond.dest, resulttemp);
			fprintf(fp, "IF %s %s %s GOTO %s\n", ltemp, &p->code.label_cond.op[0], rtemp, resulttemp);
			break;
		case CALL_FUNC:
			printOperand(&p->code.call_func.dest, resulttemp);
			fprintf(fp, "%s := CALL %s\n", resulttemp,  &p->code.call_func.func[0]);
			break;
		case ARG:
			printOperand(&p->code.arg, resulttemp);
			fprintf(fp, "ARG %s\n", resulttemp);
			break;
		case READ:
			printOperand(&p->code.read, resulttemp);
			fprintf(fp, "READ %s\n", resulttemp);
			break;
		case WRITE:
			printOperand(&p->code.write, resulttemp);
			fprintf(fp, "WRITE %s\n", resulttemp);
			break;
		case RETURN_:
			printOperand(&p->code.return_, resulttemp);
			fprintf(fp, "RETURN %s\n", resulttemp);
			break;
		case FUNCTION:
			fprintf(fp, "FUNCTION %s :\n", &p->code.function[0]);
			break;
		case PARAM:
			printOperand(&p->code.param, resulttemp);
			fprintf(fp, "PARAM %s \n", resulttemp);
			break;
		default:
			assert(0);
		}
		p = p->next;
	}
	if (OUTPUT_TO_SCREEN) {
		rewind(fp);
		char ch;
		printf("----------------------------------------------\n");
		ch = fgetc(fp);
		while (ch != EOF) {
			putchar(ch);
			ch = fgetc(fp);
		}
		printf("----------------------------------------------\n");
	}
}
