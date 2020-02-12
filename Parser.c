#include "Parser.h"

void loadInstructions(Instruction_Memory *i_mem, const char *trace)
{
    printf("Loading trace file: %s\n", trace);

    FILE *fd = fopen(trace, "r");
    if (fd == NULL)
    {
        perror("Cannot open trace file. \n");
        exit(EXIT_FAILURE);
    }

    // Iterate all the assembly instructions
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    Addr PC = 0; // program counter points to the zeroth location initially.
    int IMEM_index = 0;
    while ((read = getline(&line, &len, fd)) != -1)
    {
        // Assign program counter
        i_mem->instructions[IMEM_index].addr = PC;

        // Extract operation
        char *raw_instr = strtok(line, " ");
        if (strcmp(raw_instr, "add") == 0 ||
            strcmp(raw_instr, "sub") == 0 ||
            strcmp(raw_instr, "sll") == 0 ||
            strcmp(raw_instr, "srl") == 0 ||
            strcmp(raw_instr, "xor") == 0 ||
            strcmp(raw_instr, "or")  == 0 ||
            strcmp(raw_instr, "and") == 0)
        {
            parseRType(raw_instr, &(i_mem->instructions[IMEM_index]));
        }
        else if (strcmp(raw_instr, "ld")   == 0 ||
		         strcmp(raw_instr, "addi") == 0 ||
                 strcmp(raw_instr, "slli") == 0 ||
                 strcmp(raw_instr, "xori") == 0 ||
                 strcmp(raw_instr, "srli") == 0 ||
                 strcmp(raw_instr, "ori")  == 0 ||
                 strcmp(raw_instr, "andi") == 0 ||
                 strcmp(raw_instr, "jalr") == 0)
        {
            parseIType(raw_instr, &(i_mem->instructions[IMEM_index]));
        }
        else if (strcmp(raw_instr, "sd") == 0)
        {
            parseSType(raw_instr, &(i_mem->instructions[IMEM_index]));
        }
        else if (strcmp(raw_instr, "beq") == 0 ||
		         strcmp(raw_instr, "bne") == 0 ||
                 strcmp(raw_instr, "blt") == 0 ||
                 strcmp(raw_instr, "bge") == 0)
        {
            parseSBType(raw_instr, &(i_mem->instructions[IMEM_index]));
        }
        else if (strcmp(raw_instr, "jal") == 0)
        {
            parseUJType(raw_instr, &(i_mem->instructions[IMEM_index]));
        }
        else
        {
            printf("\033[0;31m");
            printf("ERROR: instruction \'%s\' not valid for this compiler!\n\n", raw_instr); 
            printf("\033[0m");
        }
        i_mem->last = &(i_mem->instructions[IMEM_index]);
        IMEM_index++;
        PC += 4;
    }

    fclose(fd);
}

void parseRType(char *opr, Instruction *instr)
{
    instr->instruction = 0;
    unsigned opcode = 51; // all these commands use 51
    unsigned funct3 = 0;
    unsigned funct7 = 0;
   
    if (strcmp(opr, "add") == 0){
        funct3 = 0;
        funct7 = 0;
    } else if (strcmp(opr, "sub") == 0){
        funct3 = 0;
        funct7 = 32;
    } else if (strcmp(opr, "sll") == 0){
        funct3 = 1;
        funct7 = 0;
    } else if (strcmp(opr, "srl") == 0){
        funct3 = 5;
        funct7 = 0;
    } else if (strcmp(opr, "xor") == 0){
        funct3 = 4;
        funct7 = 0;
    } else if (strcmp(opr, "or") == 0){
        funct3 = 6;
        funct7 = 32;
    } else if (strcmp(opr, "and") == 0){
        funct3 = 7;
        funct7 = 0;
    }

    char *reg = strtok(NULL, ", ");
    unsigned rd = regIndex(reg);

    reg = strtok(NULL, ", ");
    unsigned rs_1 = regIndex(reg);

    reg = strtok(NULL, ", ");
    reg[strlen(reg)-1] = '\0';
    unsigned rs_2 = regIndex(reg);

    // Contruct instruction
    instr->instruction |= opcode;
    instr->instruction |= (rd << 7);
    instr->instruction |= (funct3 << (7 + 5));
    instr->instruction |= (rs_1 << (7 + 5 + 3));
    instr->instruction |= (rs_2 << (7 + 5 + 3 + 5));
    instr->instruction |= (funct7 << (7 + 5 + 3 + 5 + 5));
}

void parseIType(char *opr, Instruction *instr){
    instr->instruction = 0;
    int opcode = 19;
    int funct3 = 0;

    if (strcmp(opr, "addi") == 0){
        funct3 = 0;
    } else if (strcmp(opr, "ld") == 0){
        opcode = 3;
        funct3 = 3;
    } else if (strcmp(opr, "slli") == 0){
        funct3 = 1;
    } else if (strcmp(opr, "xori") == 0){
        funct3 = 4;
    } else if (strcmp(opr, "srli") == 0){
        funct3 = 5;
    } else if (strcmp(opr, "ori") == 0){
        funct3 = 6;
    } else if (strcmp(opr, "andi") == 0){
        funct3 = 7;
    } else if (strcmp(opr, "jarl") == 0){
        opcode = 103;
        funct3 = 0;
    }

    char *reg = strtok(NULL, ", ");
    unsigned rd = regIndex(reg);

    unsigned rs_1 = 0;
    unsigned immediate = 0;

    //find immidiate based on opr
    if (strcmp(opr, "ld") == 0) {
        // TODO: Add specialized regex 
        // to deal with ld addressing types
        // e.g. ld x13, 48(x1)
	optcode = 3;
	funct3 = 3;
	    
        reg = strtok(NULL, ", ")
        rs_1 = regIndex(reg);   
	
	reg = strtok(NULL, ", ")
	reg[strlen(reg)-1] = '\0';  //reg = n(a)
        immediate = regIndex(reg);              //= n where n(a)
    } else {
        reg = strtok(NULL, ", ");
        rs_1 = regIndex(reg);
        
        reg = strtok(NULL, ", ");
        reg[strlen(reg)-1] = '\0';
        immediate = regIndex(reg);
    }

    instr->instruction |= opcode;
    instr->instruction |= (rd << 7);
    instr->instruction |= (funct3 << (7 + 5));
    instr->instruction |= (rs_1 << (7 + 5 + 3));
    instr->instruction |= (immediate << (7 + 5 + 3 + 5));
    
}

void parseSType(char *opr, Instruction *instr){
    instr->instruction = 0;
    unsigned opcode = 35;
    unsigned funct3 = 3;

    char *reg = strtok(NULL, ", ");
    unsigned rs_1 = regIndex(reg);

    reg = strtok(NULL, ", ");
    reg[strlen(reg)-1] = '\0';     // reg = n(a)
    
    // TODO: Fix this to accept an sd type instr
    // M[R[rs1]+imm](63:0) = R[rs2](63:0)
    // sd x14, 100(x2)
    char a = 'idk';                //= a where n(a)
    unsigned rs_2 = regIndex(a);
    unsigned immediate = 0;        //= n where n(a)

    // Contruct instruction
    instr->instruction |= opcode;
    instr->instruction |= ((immediate & 31) << 7);          //100101011101 & 11111 = 11101
    instr->instruction |= (funct3 << (7 + 5));
    instr->instruction |= (rs_1 << (7 + 5 + 3));
    instr->instruction |= (rs_2 << (7 + 5 + 3 + 5));
    instr->instruction |= ((immediate >> 5) << (5 + 5 + 3 + 5 + 7));  // 100101011101 -> 000001001010
}

void parseSBType(char *opr, Instruction *instr){
    // TODO: Write SB parser
}

void parseUJType(char *opr, Instruction *instr){
    // TODO: Write UJ parser
}


int regIndex(char *reg)
{
    unsigned i = 0;
    for (i; i < NUM_OF_REGS; i++)
    {
        if (strcmp(REGISTER_NAME[i], reg) == 0)
        {
            break;
        }
    }

    return i;
}
