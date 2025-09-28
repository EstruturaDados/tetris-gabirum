#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(*(arr)))
#define RAND_RANGE(min, max) (rand() % ((max) - (min) + 1) + (min))

#define TAMANHO_FILA 5
#define TAMANHO_PILHA 3

typedef struct peca
{
  int id;
  char tipo;
} peca_t;

typedef struct fila
{
  peca_t pecas[TAMANHO_FILA];
  int inicio, fim, total;
} fila_t;

typedef struct pilha
{
  peca_t pecas[TAMANHO_PILHA];
  int idx, total;
} pilha_t;

static void print_peca(peca_t const *peca);

static void inicializar_fila(fila_t *fila);
static bool enqueue(fila_t *fila, peca_t peca);
static bool dequeue(fila_t *fila, peca_t *peca);
static bool fila_cheia(fila_t const *fila);
static bool fila_vazia(fila_t const *fila);
static void mostrar_fila(fila_t const *fila);

static bool gerar_peca(fila_t *fila);
static void gerar_pecas(fila_t *fila);

static void inicializar_pilha(pilha_t *pilha);
static bool push(pilha_t *pilha, peca_t peca);
static bool pop(pilha_t *pilha, peca_t *peca);
static bool pilha_cheia(pilha_t const *pilha);
static bool pilha_vazia(pilha_t const *pilha);
static void mostrar_pilha(pilha_t const *pilha);

static bool swap(fila_t *fila, pilha_t *pilha, int quantidade);

static char const *MENU = "Ações:\n"
                          "\t1 - Jogar peça\n"
                          "\t2 - Enviar peça da filha para a pilha (reserva)\n"
                          "\t3 - Usar peça da reserva (remover do topo da pilha)\n"
                          "\t4 - Trocar peça da frente com topo da pilha\n"
                          "\t5 - Trocar 3 primeiros da fila com os 3 da pilha\n"
                          "\t0 - Sair";
static char const TIPOS[] = "IOTL";
static int id_atual = 0;

int main()
{
  srand(time(NULL));

  fila_t fila;
  pilha_t pilha;
  inicializar_fila(&fila);
  inicializar_pilha(&pilha);
  gerar_pecas(&fila);

  int op = 0;
  do
  {
    mostrar_fila(&fila);
    mostrar_pilha(&pilha);
    puts(MENU);
    putchar('>');
    scanf("%d", &op);

    switch (op)
    {
    case 1:
    {
      peca_t peca;
      if (!dequeue(&fila, &peca))
      {
        fputs("Não foi possível pegar uma peça\n", stderr);
        break;
      }
      fputs("Peça jogada: ", stdout);
      print_peca(&peca);
      putchar('\n');
      if (!gerar_peca(&fila))
      {
        fputs("Não foi possível gerar outra peça para a fila\n", stderr);
      }
    }
    break;

    case 2:
    {
      if (pilha_cheia(&pilha))
      {
        fputs("Pilha cheia\n", stderr);
        break;
      }

      peca_t peca;
      if (!dequeue(&fila, &peca))
      {
        fputs("Não foi possível pegar uma peça\n", stderr);
        break;
      }
      if (!push(&pilha, peca))
      {
        fputs("Não foi possível adicionar peça na pilha\n", stderr);
      }
      if (!gerar_peca(&fila))
      {
        fputs("Não foi possível gerar outra peça para a fila\n", stderr);
      }
    }
    break;

    case 3:
    {
      peca_t peca;
      if (!pop(&pilha, &peca))
      {
        fputs("Não foi possível usar peça da reserva\n", stderr);
        break;
      }
      fputs("Peça jogada: ", stdout);
      print_peca(&peca);
      putchar('\n');
    }
    break;

    case 4:
    case 5:
      if (!swap(&fila, &pilha, op == 4 ? 1 : 3))
      {
        fputs("Não foi possível realizar operação de troca\n", stderr);
      }
      break;

    default:
      break;
    }
  } while (op != 0);

  return 0;
}

void print_peca(peca_t const *peca)
{
  printf("[%c %d]", peca->tipo, peca->id);
}

static void inicializar_fila(fila_t *fila)
{
  fila->inicio = 0;
  fila->fim = 0;
  fila->total = 0;
}

static bool enqueue(fila_t *fila, peca_t peca)
{
  if (fila_cheia(fila))
  {
    return false;
  }

  fila->pecas[fila->fim] = peca;
  fila->fim = (fila->fim + 1) % TAMANHO_FILA;
  fila->total++;

  return true;
}

static bool dequeue(fila_t *fila, peca_t *peca)
{
  if (fila_vazia(fila))
  {
    return false;
  }

  memcpy(peca, &fila->pecas[fila->inicio], sizeof(peca_t));
  fila->inicio = (fila->inicio + 1) % TAMANHO_FILA;
  fila->total--;

  return true;
}

static bool fila_cheia(fila_t const *fila)
{
  return fila->total >= TAMANHO_FILA;
}

static bool fila_vazia(fila_t const *fila)
{
  return fila->total <= 0;
}

void mostrar_fila(fila_t const *fila)
{
  if (fila_vazia(fila))
  {
    fputs("Fila vazia\n", stderr);
    return;
  }

  printf("Fila: {");
  int fi = fila->inicio;
  for (int i = 0; i < fila->total; i++)
  {
    print_peca(&fila->pecas[fi]);
    if (i < fila->total - 1)
    {
      fputs(", ", stdout);
    }
    fi = (fi + 1) % TAMANHO_FILA;
  }
  puts("}");
}

bool gerar_peca(fila_t *fila)
{
  int tipo_idx = RAND_RANGE(0, ARRAY_SIZE(TIPOS) - 2);
  peca_t peca;
  peca.id = id_atual++;
  peca.tipo = TIPOS[tipo_idx];
  return enqueue(fila, peca);
}

static void gerar_pecas(fila_t *fila)
{
  while (gerar_peca(fila))
    ;
}

static void inicializar_pilha(pilha_t *pilha)
{
  pilha->idx = -1;
  pilha->total = 0;
}

static bool push(pilha_t *pilha, peca_t peca)
{
  if (pilha_cheia(pilha))
  {
    return false;
  }

  pilha->pecas[++pilha->idx] = peca;
  pilha->total++;

  return true;
}

static bool pop(pilha_t *pilha, peca_t *peca)
{
  if (pilha_vazia(pilha))
  {
    return false;
  }

  memcpy(peca, &pilha->pecas[pilha->idx--], sizeof(peca_t));
  pilha->total--;

  return true;
}

static bool pilha_cheia(pilha_t const *pilha)
{
  return pilha->total >= TAMANHO_PILHA;
}

bool pilha_vazia(pilha_t const *pilha)
{
  return pilha->total <= 0;
}

void mostrar_pilha(pilha_t const *pilha)
{
  if (pilha_vazia(pilha))
  {
    fputs("Pilha vazia\n", stderr);
    return;
  }

  printf("Pilha: {");
  for (int i = pilha->idx; i >= 0; i--)
  {
    print_peca(&pilha->pecas[i]);
    if (i != 0)
    {
      printf(", ");
    }
  }
  puts("}");
}

static bool swap(fila_t *fila, pilha_t *pilha, int quantidade)
{
  if (quantidade < 0)
  {
    fprintf(stderr, "Quantidade inválida: %d\n", quantidade);
    return false;
  }

  if (quantidade > TAMANHO_FILA || quantidade > TAMANHO_PILHA)
  {
    fprintf(stderr, "A quantidade supera o tamanho das estruturas fila e pilha, respectivamente, %d e %d\n", TAMANHO_FILA, TAMANHO_PILHA);
    return false;
  }

  if (quantidade > fila->total || quantidade > pilha->total)
  {
    fprintf(stderr, "Não há itens suficientes para a troca (fila: %d, pilha: %d)\n", fila->total, pilha->total);
    return false;
  }

  for (int i = 0, fi = fila->inicio, pi = pilha->idx;
       i < quantidade && fi >= 0 && fi < TAMANHO_FILA && pi >= 0;
       i++, fi = (fi + 1) % TAMANHO_FILA, pi--)
  {
    peca_t peca = fila->pecas[fi];
    fila->pecas[fi] = pilha->pecas[pi];
    pilha->pecas[pi] = peca;
  }

  return true;
}
