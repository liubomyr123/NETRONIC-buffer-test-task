#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <cstring>
#include <algorithm>
#define uint8_t unsigned char
#define EMPTY_VALUE 0xFF

// дано циклічний (кільцевий) буфер та деякі функції роботи з ним
#define BUFFER_SIZE 8

#if (BUFFER_SIZE & (BUFFER_SIZE - 1)) != 0
  #error "Incorrect buffer size"
#endif

typedef struct {
  size_t read_ptr;
  size_t write_ptr;
  uint8_t data[BUFFER_SIZE];
} CircularBuffer;

// ClearBuf очищає буфер (може також використовуватися для ініціалізації структури CircularBuffer)
void ClearBuf(CircularBuffer* pBuf)
{
  pBuf->read_ptr = 0;
  pBuf->write_ptr = 0;

  for (size_t i = 0; i < BUFFER_SIZE; i++)
    pBuf->data[i] = EMPTY_VALUE;
}

// ReadByte читає байт з буфера. Якщо в буфері немає даних, повертає -1.
int ReadByte(CircularBuffer* pBuf)
{
  if (pBuf->read_ptr == pBuf->write_ptr)
    return -1;
  int result = pBuf->data[pBuf->read_ptr];
  pBuf->read_ptr = (pBuf->read_ptr + 1) & (BUFFER_SIZE - 1);
  return result;
}

// пише байт в буфер, повертає true якщо запис відбувся успішно
bool WriteByte(CircularBuffer* pBuf, uint8_t value)
{
  size_t next = (pBuf->write_ptr + 1) & (BUFFER_SIZE - 1);
  if (next == pBuf->read_ptr)
    return false;
  pBuf->data[pBuf->write_ptr] = value;
  pBuf->write_ptr = next;
  return true;
}

// функція IsEmpty повертає true якщо буфер пустий, в іншому випадку - false
// пустим є буфер в якому немає даних для читання.
bool IsEmpty(CircularBuffer* pBuf)
{
  // TODO: напишіть код цієї фукнції
  if (pBuf->read_ptr == pBuf->write_ptr)
  {
    return true;
  }
  return false;
}

// функція IsFull повертає true якщо буфер полон, в іншому випадку - false
// спроба писати в повний буфер завжди буде закінчуватися невдачею.
bool IsFull(CircularBuffer* pBuf)
{
  // TODO: напишіть код цієї фукнції
  size_t next = (pBuf->write_ptr + 1) & (BUFFER_SIZE - 1);
  if (next == pBuf->read_ptr)
  {
    return true;
  }
  return false;
}

// BUFFER_SIZE = 8
// read_ptr  = 0
// write_ptr = 5
//
// [0, 1, 2, 3, 4, 5, 6, 7]
// [x, x, x, x, x, _, _, _]
//  ↑              ↑
// read           write


// BUFFER_SIZE = 8
// read_ptr  = 2
// write_ptr = 5
//
// [0, 1, 2, 3, 4, 5, 6, 7]
// [_, _, x, x, x, _, _, _]
//        ↑        ↑
//       read     write


// BUFFER_SIZE = 8
// read_ptr  = 5
// write_ptr = 2
//
// [0, 1, 2, 3, 4, 5, 6, 7]
// [x, x, _, _, _, x, x, x]
//        ↑        ↑
//       write     read


// 11111101
// &
// 00000111
// -----------------------------------
// 00000101 => 5

// що повертає функція GetSomething? перейменуйте її, щоб назва відповідала значеню що повертається 
size_t GetBufferSize(CircularBuffer* pBuf)
{
  return (pBuf->write_ptr - pBuf->read_ptr) & (BUFFER_SIZE - 1);
}

// нам потрібна функція для переміщення даних з одного циклічного буфера в інший
// ми вирішили її обявити так
// size_t BufMove(CicrularBufffer* pDest, CicrularBufffer* pSource)
// pDest и pSource вказують на різні буфери
// функція повинна перемістити максимальну можливу кількість байтів з Source в Dest
// і повертати число скопійований байт.
// тобто якщо в буфері Dest не вистачає місця для всього вмісту з Source,
// мають переміститися лише ті байти, для яких є місце; решта повинна залишитися в буфері Source. 
// якщо ж місця достатньо, має переміститися все, і буфер Source залишиться порожнім.

// програміст написав таку функцію
// чи відповідає вона опису, наведеному вище?
// які в неї є недоліки?
size_t BufMoveSlow(CircularBuffer* pDest, CircularBuffer* pSource)
{
  if (pDest == pSource)
    return 0;
  int value;
  size_t result = 0;
  while ((value = ReadByte(pSource)) != -1 && WriteByte(pDest, value))      // якщо pDest вже повний, то ми будемо втрачати дані з pSource, тому треба спочатку перевіряти на заповненість обох буферів
    result++;                                                               // очевидно що ми робимо багато викликів, багато копіювань і вставок, це не ефективно
  return result;
}

size_t GetAvaliableSize(CircularBuffer* pBuf)
{
  size_t occupiedSize = GetBufferSize(pBuf);
  return (BUFFER_SIZE - 1) - occupiedSize;
}

bool IsIndexInData(CircularBuffer* pBuf, size_t idx)
{
  if (pBuf->read_ptr == pBuf->write_ptr)
    return false;

  if (pBuf->read_ptr < pBuf->write_ptr)
  {
    return (idx >= pBuf->read_ptr && idx < pBuf->write_ptr);
  }
  else
  {
    return (idx >= pBuf->read_ptr || idx < pBuf->write_ptr);
  }
}

void PrintBufferArray(CircularBuffer* pBuf) 
{ 
  // size_t read_ptr = pBuf->read_ptr;
  // size_t write_ptr = pBuf->write_ptr;

  // printf("read_ptr: %x\n", read_ptr);
  // printf("write_ptr: %x\n", write_ptr);

  printf("[");
  for (size_t i = 0; i < BUFFER_SIZE; i++)
  {
    size_t data = pBuf->data[i];

    if (data == EMPTY_VALUE)
    {
      printf("_");
    }
    else
    {
      if (IsIndexInData(pBuf, i))
      {
        printf("%d", data);
      }
      else
      {
        printf("?");
      }
    }
    if (i < BUFFER_SIZE-1)
    {
      printf(",");
    }
  }
  printf("]\n");
}

// напишіть свій варіант функції переміщення даних
size_t BufMoveFast(CircularBuffer* pDest, CircularBuffer* pSource)
{
  if (pDest == pSource)
  {
    printf("[Skip] Same buffer");
    printf("\n");
    return 0;
  }

  if (IsEmpty(pSource))
  {
    printf("[Skip] Source is empty");
    printf("\n");
    return 0;
  }

  if (IsFull(pDest))
  {
    printf("[Skip] Dest is full");
    printf("\n");
    return 0;
  }

  printf("\n[Before moving]\n");

  printf("  Source before move: ");
  PrintBufferArray(pSource);

  printf("  Dest before move:   ");
  PrintBufferArray(pDest);

  size_t totalCopied = 0;

  size_t sourceDataSize = GetBufferSize(pSource);
  size_t destDataSize = GetBufferSize(pDest);

  size_t sourceFreeSize = GetAvaliableSize(pSource);
  size_t destFreeSize = GetAvaliableSize(pDest);

  size_t toCopy = std::min(sourceDataSize, destFreeSize);

  printf("  Total will be copied: %zu\n", toCopy);

  size_t count = 1;
  while (toCopy > 0)
  {
    printf("\n[Iteration #%d]\n", count);
    
    printf("  Source: ");
    PrintBufferArray(pSource);
    
    printf("  Dest:   ");
    PrintBufferArray(pDest);
    printf("\n");
    
    sourceDataSize = GetBufferSize(pSource);
    destDataSize = GetBufferSize(pDest);

    sourceFreeSize = GetAvaliableSize(pSource);
    destFreeSize = GetAvaliableSize(pDest);

    printf("  Source  -> Occupied: %x, Free: %x\n", sourceDataSize, sourceFreeSize);
    printf("  Dest    -> Occupied: %x, Free: %x\n", destDataSize, destFreeSize);
    printf("\n");
    printf("  Source  -> read_ptr: %x, write_ptr: %x\n", pSource->read_ptr, pSource->write_ptr);
    printf("  Dest    -> read_ptr: %x, write_ptr: %x\n", pDest->read_ptr, pDest->write_ptr);
    printf("\n");

    size_t src_start = pSource->read_ptr;

    size_t src_len;
    if (pSource->read_ptr < pSource->write_ptr)
    {
      // [0, 1, 2, 3, 4, 5, 6, 7]
      // [_, _, x, x, x, _, _, _]
      //        ↑        ↑
      //       read     write
      src_len = pSource->write_ptr - pSource->read_ptr;
    }
    else
    {
      // [0, 1, 2, 3, 4, 5, 6, 7]
      // [x, x, _, _, _, x, x, x]
      //        ↑        ↑
      //       write     read
      src_len = BUFFER_SIZE - pSource->read_ptr;
    }

    size_t dest_space_until_end = BUFFER_SIZE - pDest->write_ptr;

    size_t chunk = std::min({src_len, toCopy, dest_space_until_end});

    printf("  Can be copied from source: %zu\n", src_len);
    printf("  Elements in dest from [write] to end: %zu\n", dest_space_until_end);
    printf("  Free elements in dest: %zu\n", destFreeSize);
    printf("\n");
    printf("  Will be copied: %zu\n", chunk);

    printf("\n");
    printf("  Copying...\n");
    memcpy(
      pDest->data + pDest->write_ptr,
      pSource->data + src_start,
      chunk
    );

    pDest->write_ptr = (pDest->write_ptr + chunk) & (BUFFER_SIZE - 1);
    pSource->read_ptr = (pSource->read_ptr + chunk) & (BUFFER_SIZE - 1);

    toCopy -= chunk;
    totalCopied += chunk;

    printf("\n");
    printf("  Copied: %zu, Remaining: %zu\n", chunk, toCopy);
    count++;
  }

  printf("\n[Result]\n");
  printf("  Total copied: %zu\n", totalCopied);

  printf("\n");
  printf("  Source after move:  ");
  PrintBufferArray(pSource);

  printf("  Dest after move:    ");
  PrintBufferArray(pDest);
  printf("\n");

  return totalCopied;
}

// допоміжна функція для дебагу
void PrintBuffer(CircularBuffer* pBuf) 
{ 
  if (pBuf->read_ptr == pBuf->write_ptr)
    printf(" Empty\n");
  size_t pos;

  for (pos = pBuf->read_ptr; pos != pBuf->write_ptr; pos = (pos + 1) & (BUFFER_SIZE - 1))
  {
    printf("[%d]:%02x\n", pos, pBuf->data[pos]);
  }

  printf("\n");
}

// код нижче можна змінювати на свій розсуд для тестування функції
bool CheckEqual(CircularBuffer* buf, std::initializer_list<uint8_t> expected)
{
  size_t i = buf->read_ptr;
  for (auto val : expected)
  {
    if (i == buf->write_ptr || buf->data[i] != val)
      return false;
    i = (i + 1) & (BUFFER_SIZE - 1);
  }
  return i == buf->write_ptr;
}

void PrintTestResult(const char* name, bool ok)
{
  printf("[%s] %s\n", name, ok ? "OK" : "FAIL");
}

void Test_SimpleMove()
{
  printf("[Test_SimpleMove]:\n");

  CircularBuffer A, B;
  ClearBuf(&A);
  ClearBuf(&B);

  WriteByte(&A, 1);
  WriteByte(&A, 2);
  WriteByte(&A, 3);

  printf("Moving from BufferA to bufferB...\n");
  size_t moved = BufMoveFast(&B, &A);

  bool ok = (moved == 3)
    && CheckEqual(&A, {})
    && CheckEqual(&B, {1,2,3});

  PrintTestResult("Test_SimpleMove", ok);
}

void Test_PartialMove()
{
  printf("[Test_PartialMove]:\n");
 
  CircularBuffer A, B;
  ClearBuf(&A);
  ClearBuf(&B);

  for (int i = 1; i <= 5; i++) // [1,2,3,4,5,_,_,_]
    WriteByte(&A, i);

  for (int i = 10; i < 15; i++) // [10,11,12,13,14,_,_,_]
    WriteByte(&B, i);

  printf("Moving from BufferA to bufferB...\n");
  size_t moved = BufMoveFast(&B, &A);

  bool ok = (moved == 2)
    && CheckEqual(&A, {3,4,5})
    && CheckEqual(&B, {10,11,12,13,14,1,2});

  PrintTestResult("Test_PartialMove", ok);
}

void Test_SourceWrap()
{
  printf("[Test_SourceWrap]:\n");
 
  CircularBuffer A, B;
  ClearBuf(&A);
  ClearBuf(&B);

  for (int i = 0; i < 6; i++) // [0,1,2,3,4,5,_,_]
    WriteByte(&A, i);

  for (int i = 0; i < 4; i++) // [?,?,?,?,4,5,_,_]
    ReadByte(&A);

  WriteByte(&A, 100);
  WriteByte(&A, 101);
  // ->
  // [?,?,?,?,4,5,100,101]

  printf("Moving from BufferA to bufferB...\n");
  size_t moved = BufMoveFast(&B, &A);

  bool ok = (moved == 4)
    && CheckEqual(&A, {})
    && CheckEqual(&B, {4,5,100,101});

  PrintTestResult("Test_SourceWrap", ok);
}

void Test_DestWrap()
{
  printf("[Test_DestWrap]:\n");
  
  CircularBuffer A, B;
  ClearBuf(&A);
  ClearBuf(&B);

  for (int i = 1; i <= 4; i++) // [1,2,3,4,_,_,_,_]
    WriteByte(&A, i);

  for (int i = 10; i <= 15; i++) // [10,11,12,13,14,15,_,_]
    WriteByte(&B, i);

  for (int i = 0; i < 4; i++) // [?,?,?,?,14,15,_,_]
    ReadByte(&B);

  printf("Moving from BufferA to bufferB...\n");
  size_t moved = BufMoveFast(&B, &A);

  bool ok = (moved == 4)
    && CheckEqual(&A, {})
    && CheckEqual(&B, {14,15,1,2,3,4});

  PrintTestResult("Test_DestWrap", ok);
}

void Test_BothWrap()
{
  printf("[Test_BothWrap]:\n");
  
  CircularBuffer A, B;
  ClearBuf(&A);
  ClearBuf(&B);

  // source wrap
  for (int i = 0; i < 6; i++) // [0,1,2,3,4,5,_,_]
    WriteByte(&A, i);

  for (int i = 0; i < 4; i++) // [?,?,?,?,4,5,_,_]
    ReadByte(&A);

  WriteByte(&A, 100);
  WriteByte(&A, 101);
  // ->
  // [?,?,?,?,4,5,100,101]

  // dest wrap
  for (int i = 10; i < 16; i++) // [10,11,12,13,14,15,_,_]
    WriteByte(&B, i);
 
  for (int i = 0; i < 3; i++) // [?,?,?,13,14,15,_,_]
    ReadByte(&B);

  printf("Moving from BufferA to bufferB...\n");
  size_t moved = BufMoveFast(&B, &A);

  bool ok = (moved > 0); // тут можна деталізувати

  PrintTestResult("Test_BothWrap", ok);
}

void Test_NoMove()
{
  printf("[Test_NoMove]:\n");

  CircularBuffer A, B;
  ClearBuf(&A);
  ClearBuf(&B);

  printf("Moving from BufferA to bufferB...\n");
  size_t moved = BufMoveFast(&B, &A);

  bool ok = (moved == 0);

  PrintTestResult("Test_NoMove", ok);
}

int main(){
  Test_SimpleMove();
  Test_PartialMove();
  Test_SourceWrap();
  Test_DestWrap();
  Test_BothWrap();
  Test_NoMove();

  return 0;
}

















