#include "include/core.h"

void db_handle(FILE *f, char *line)
{
  InstrArray *instrarray = parse_user_input(line);
  if (instrarray)
  {
    dump_instrarray(f, instrarray);
    for (int i = 0; i < instrarray->size; ++i)
    {
      instr *instr = instrarray->arr[i];
      struct CrtInstr *crtinstr;
      struct SelInstr *selinstr;
      struct AddInstr *addinstr;
      switch (instr->type)
      {
      case unknownInstrType:
        break;
      case sel:
        selinstr = (struct SelInstr *)instr;
        if (selinstr->has_cond)
        {
          fprintf(f, "%s\n\n",
                  select_where(selinstr->table,
                               selinstr->columns->arr,
                               selinstr->columns->size,
                               selinstr->cond->col,
                               record_from_string(&selinstr->cond->val)));
        }
        else
        {
          fprintf(f, "%s\n\n",
                  select_all(selinstr->table,
                             selinstr->columns->arr,
                             selinstr->columns->size));
        }
        break;
      case crt:
        crtinstr = (struct CrtInstr *)instr;
        Page page = create_table(crtinstr->table,
                                 crtinstr->columns->arr,
                                 crtinstr->columns->size);
        if (page < 0)
          fprintf(f, "ERROR: Unable to create table\n\n");
        else
          fprintf(f, "Table created\n\n");
        break;
      case add:
      {
        addinstr = (struct AddInstr *)instr;
        short sz = addinstr->columns->size;
        struct record *records[sz];
        Key root_id = get_table_id(addinstr->table);
        short *table_id = get_column_offsets(root_id, addinstr->columns->arr, addinstr->columns->size);
        for (int i = 0; i < sz; ++i)
          records[table_id[i]] = record_from_string(&addinstr->values->arr[i]);
        free(table_id);
        Key id = advanced_insert_row(root_id, records, sz);
        for (size_t i = 0; i < sz; i++)
        {
          free(records[i]->data);
          free(records[i]);
        }
        fprintf(f, "Entry added at index %ld\n\n", id);
      }
      break;
      default:
        break;
      }
    }
    free_instrarray(instrarray);
    instrarray = NULL;
  }
  else
  {
    // Should be impossible to reach
    fprintf(f, "ERR: parsing failed");
  }
}

void init_database(){
  printf("Initializing Database...\n");
  initialize_tables();
}