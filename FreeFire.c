/*
 * Torre de Fuga — Organização de Componentes
 * - Ordenações: Bubble (nome), Insertion (tipo), Selection (prioridade)
 * - Busca binária: por nome (apenas após ordenar por nome)
 * - Métricas: nº de comparações e tempo de execução (clock)
 *
 * Compilar: gcc -std=c99 -Wall -Wextra -O2 torre.c -o torre
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_COMP 20

typedef struct {
    char nome[30];     // ex: "chip central"
    char tipo[20];     // ex: "controle", "suporte", "propulsao"
    int  prioridade;   // 1..10 (10 = maior prioridade)
} Componente;

/* ===== Utilidades ===== */

static void limparNL(char *s) {
    if (!s) return;
    size_t n = strlen(s);
    if (n && s[n-1] == '\n') s[n-1] = '\0';
}

static int lerLinha(const char *prompt, char *buf, size_t tam) {
    printf("%s", prompt);
    if (!fgets(buf, (int)tam, stdin)) return 0;
    limparNL(buf);
    return 1;
}

static int lerInt(const char *prompt, int *out, int minv, int maxv) {
    char tmp[64];
    while (1) {
        printf("%s", prompt);
        if (!fgets(tmp, sizeof(tmp), stdin)) return 0;
        char *end;
        long v = strtol(tmp, &end, 10);
        if (end == tmp || (*end && *end!='\n')) {
            printf("Entrada inválida. Tente novamente.\n");
            continue;
        }
        if (v < minv || v > maxv) {
            printf("Valor fora do intervalo [%d..%d].\n", minv, maxv);
            continue;
        }
        *out = (int)v;
        return 1;
    }
}

/* ===== Exibição ===== */

void mostrarComponentes(Componente v[], int n) {
    puts("\n=== Componentes ===");
    for (int i = 0; i < n; i++) {
        printf("[%02d] Nome: %-28s | Tipo: %-18s | Prioridade: %d\n",
               i+1, v[i].nome, v[i].tipo, v[i].prioridade);
    }
    puts("===================\n");
}

/* ===== Ordenações =====
 * Cada função recebe um ponteiro para contador de comparações.
 * Contamos 1 por comparação de chave (strcmp ou < de prioridade).
 */

void bubbleSortNome(Componente a[], int n, long *comparacoes) {
    *comparacoes = 0;
    for (int i = 0; i < n - 1; i++) {
        int trocou = 0;
        for (int j = 0; j < n - 1 - i; j++) {
            (*comparacoes)++; // comparação de nomes
            if (strcmp(a[j].nome, a[j+1].nome) > 0) {
                Componente tmp = a[j];
                a[j] = a[j+1];
                a[j+1] = tmp;
                trocou = 1;
            }
        }
        if (!trocou) break;
    }
}

void insertionSortTipo(Componente a[], int n, long *comparacoes) {
    *comparacoes = 0;
    for (int i = 1; i < n; i++) {
        Componente key = a[i];
        int j = i - 1;
        // fazemos a primeira comparação fora do loop se existir
        while (j >= 0) {
            (*comparacoes)++; // comparação de tipos
            if (strcmp(a[j].tipo, key.tipo) > 0) {
                a[j+1] = a[j];
                j--;
            } else {
                break;
            }
        }
        a[j+1] = key;
    }
}

void selectionSortPrioridade(Componente a[], int n, long *comparacoes) {
    *comparacoes = 0;
    for (int i = 0; i < n - 1; i++) {
        int maxIdx = i; // prioridade maior vem primeiro (ordem desc)
        for (int j = i + 1; j < n; j++) {
            (*comparacoes)++; // comparação de prioridades
            if (a[j].prioridade > a[maxIdx].prioridade) {
                maxIdx = j;
            }
        }
        if (maxIdx != i) {
            Componente tmp = a[i];
            a[i] = a[maxIdx];
            a[maxIdx] = tmp;
        }
    }
}

/* ===== Busca binária por nome =====
 * Pré-condição: vetor ordenado por nome (crescente).
 * Retorna índice encontrado ou -1. Conta comparações (strcmp).
 */
int buscaBinariaPorNome(Componente a[], int n, const char chave[], long *comparacoes) {
    *comparacoes = 0;
    int lo = 0, hi = n - 1;
    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2;
        (*comparacoes)++;
        int cmp = strcmp(a[mid].nome, chave);
        if (cmp == 0) return mid;
        if (cmp < 0) lo = mid + 1;
        else hi = mid - 1;
    }
    return -1;
}

/* ===== Medição de tempo =====
 * Executa o algoritmo de ordenação e mede o tempo decorrido (segundos).
 * A ordenação é feita "in place" no vetor principal, como esperado.
 */
double medirTempo(void (*alg)(Componente*, int, long*), Componente v[], int n, long *comparacoes) {
    clock_t ini = clock();
    alg(v, n, comparacoes);
    clock_t fim = clock();
    return (double)(fim - ini) / CLOCKS_PER_SEC;
}

/* ===== Programa principal ===== */

int main(void) {
    Componente comps[MAX_COMP];
    int n = 0;
    int ordenadoPorNome = 0;

    puts("=== Torre de Fuga — Gerenciador de Componentes ===");

    // Cadastro inicial (até 20)
    if (!lerInt("Quantos componentes deseja cadastrar (1..20)? ", &n, 1, MAX_COMP)) return 0;
    for (int i = 0; i < n; i++) {
        printf("\n-- Componente %d/%d --\n", i+1, n);

        while (1) {
            if (!lerLinha("Nome (até 29 chars): ", comps[i].nome, sizeof(comps[i].nome))) return 0;
            if (strlen(comps[i].nome) == 0) { puts("Nome não pode ser vazio."); continue; }
            break;
        }

        while (1) {
            if (!lerLinha("Tipo (até 19 chars): ", comps[i].tipo, sizeof(comps[i].tipo))) return 0;
            if (strlen(comps[i].tipo) == 0) { puts("Tipo não pode ser vazio."); continue; }
            break;
        }

        if (!lerInt("Prioridade (1..10): ", &comps[i].prioridade, 1, 10)) return 0;
    }

    int opc;
    do {
        puts("\n=== Menu ===");
        puts("1) Ordenar por NOME (Bubble)  + métricas");
        puts("2) Ordenar por TIPO (Insertion) + métricas");
        puts("3) Ordenar por PRIORIDADE (Selection) + métricas");
        puts("4) Buscar componente-chave por NOME (binária)");
        puts("5) Mostrar componentes");
        puts("0) Sair");
        if (!lerInt("Escolha: ", &opc, 0, 5)) return 0;

        if (opc == 1) {
            long comp = 0;
            double t = medirTempo(bubbleSortNome, comps, n, &comp);
            ordenadoPorNome = 1;
            puts("\n>> Ordenado por NOME (Bubble).");
            printf("Comparações: %ld | Tempo: %.6f s\n", comp, t);
            mostrarComponentes(comps, n);

        } else if (opc == 2) {
            long comp = 0;
            double t = medirTempo(insertionSortTipo, comps, n, &comp);
            ordenadoPorNome = 0;
            puts("\n>> Ordenado por TIPO (Insertion).");
            printf("Comparações: %ld | Tempo: %.6f s\n", comp, t);
            mostrarComponentes(comps, n);

        } else if (opc == 3) {
            long comp = 0;
            double t = medirTempo(selectionSortPrioridade, comps, n, &comp);
            ordenadoPorNome = 0;
            puts("\n>> Ordenado por PRIORIDADE (Selection — desc).");
            printf("Comparações: %ld | Tempo: %.6f s\n", comp, t);
            mostrarComponentes(comps, n);

        } else if (opc == 4) {
            if (!ordenadoPorNome) {
                puts("\nA busca binária só é válida após ordenar por NOME (opção 1).");
                puts("Dica: ordene por nome e tente novamente.");
                continue;
            }
            char chave[30];
            if (!lerLinha("Digite o NOME do componente-chave: ", chave, sizeof(chave))) return 0;
            long compsBin = 0;
            int idx = buscaBinariaPorNome(comps, n, chave, &compsBin);
            printf("Comparações (binária): %ld\n", compsBin);
            if (idx >= 0) {
                puts(">> Componente-chave ENCONTRADO! Confirmação visual:");
                printf(" - Nome: %s | Tipo: %s | Prioridade: %d\n",
                       comps[idx].nome, comps[idx].tipo, comps[idx].prioridade);
            } else {
                puts(">> Componente-chave NÃO encontrado.");
            }

        } else if (opc == 5) {
            mostrarComponentes(comps, n);
        }

    } while (opc != 0);

    puts("\nMissão encerrada. Boa sorte na extração! 🛡️");
    return 0;
}
