#include "lcab_config.h"
#include "cstruct.h"

void lcab_defaults(struct lcab_config *config);
int write_cab_file(struct lcab_config *config);
void add_input_file(struct lcab_config *config, char *real_path, char *cab_path);
void cleanup(struct lcab_config *config);
void cab_output_path(struct lcab_config *config, char *buffer);
