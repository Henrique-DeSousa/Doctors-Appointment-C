#ifndef __CONSULTA_H__
#define __CONSULTA_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
 int type; // Tipo de Consulta: 1-Normal, 2-COVID19, 3-Urgente
 char description[100]; // Descrição da Consulta
 int pid_consult; // PID do processo que quer fazer a consulta
} Consulta;

#endif
