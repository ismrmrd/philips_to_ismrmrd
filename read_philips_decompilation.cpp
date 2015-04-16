int getBytes(PyFI::Array<unsigned char>&, int, bool, int*, int*, int*)(void * arg0, int arg1, bool arg2, int * arg3, int * arg4, int * arg5) {
  r9 = arg5;
  r8 = arg4;
  rdx = arg2;
  rsi = arg1;
  rdi = arg0;
  r10 = arg3;
  LODWORD(rbx) = *(int32_t *)r8;
  if (LODWORD(rbx) != 0x0) {
    LODWORD(rax) = *(int32_t *)r10;
    LODWORD(rax) = LODWORD(rax) << LOBYTE(LODWORD(0x20) - LODWORD(rbx));
    LODWORD(rbx) = LODWORD(rbx) - LODWORD(rsi);
    if (LODWORD(rbx) < 0x0) {
      r14 = sign_extend_64(*(int32_t *)r9);
      rdi = *(rdi + 0x20);
      LODWORD(rbx) = *(int8_t *)(rdi + sign_extend_64(LODWORD(r14 + 0x3))) & 0xff;
      LODWORD(rcx) = *(int8_t *)(rdi + sign_extend_64(LODWORD(r14 + 0x2))) & 0xff;
      LODWORD(rcx) = LODWORD(LODWORD(rcx) << 0x10) | LODWORD(LODWORD(rbx) << 0x18);
      LODWORD(rbx) = *(int8_t *)(rdi + sign_extend_64(LODWORD(r14 + 0x1))) & 0xff;
      LODWORD(rdi) = *(int8_t *)(rdi + r14) & 0xff;
      LODWORD(rdi) = LODWORD(rdi) | LODWORD(LODWORD(LODWORD(rbx) << 0x8) | LODWORD(rcx));
      *(int32_t *)r9 = LODWORD(r14 + 0x4);
      *(int32_t *)r10 = LODWORD(rdi);
      LODWORD(rcx) = *(int32_t *)r8;
      LODWORD(rax) = LODWORD(rax) | LODWORD(LODWORD(rdi) >> LOBYTE(rcx));
      LODWORD(r11) = LODWORD(0x20) - LODWORD(rsi);
      *(int32_t *)r8 = LODWORD(LODWORD(rcx) + LODWORD(r11));
    }
    else {
      *(int32_t *)r8 = LODWORD(rbx);
      LODWORD(r11) = LODWORD(0x20) - LODWORD(rsi);
    }
  }
  else {
    r11 = sign_extend_64(*(int32_t *)r9);
    rdi = *(rdi + 0x20);
    LODWORD(rax) = *(int8_t *)(rdi + sign_extend_32(LODWORD(r11 + 0x3))) & 0xff;
    LODWORD(rbx) = *(int8_t *)(rdi + sign_extend_64(LODWORD(r11 + 0x2))) & 0xff;
    LODWORD(rcx) = *(int8_t *)(rdi + sign_extend_32(LODWORD(r11 + 0x1))) & 0xff;
    LODWORD(rcx) = LODWORD(LODWORD(rcx) << 0x8) | LODWORD(LODWORD(LODWORD(rbx) << 0x10) | LODWORD(LODWORD(rax) << 0x18));
    LODWORD(rax) = *(int8_t *)(rdi + r11) & 0xff;
    LODWORD(rax) = LODWORD(rax) | LODWORD(rcx);
    *(int32_t *)r9 = LODWORD(r11 + 0x4);
    *(int32_t *)r10 = LODWORD(rax);
    LODWORD(r11) = LODWORD(0x20) - LODWORD(rsi);
    *(int32_t *)r8 = LODWORD(r11);
  }
  LODWORD(rsi) = SAR(LODWORD(rax), LOBYTE(r11));
  LODWORD(rax) = LODWORD(rax) >> LOBYTE(r11);
  if (LOBYTE(rdx) != 0x0) {
    LODWORD(rax) = LODWORD(rsi);
  }
  return rax;
}


int decode(PyFI::Array<unsigned char>&, PyFI::Array<int>&, int, int, int*)(void * arg0, void * arg1, int arg2, int arg3, int * arg4) {
  rcx = arg3;
  rdx = arg2;
  rdi = arg0;
  r9 = arg4;
  var_48 = r9;
  var_58 = arg1;
  var_50 = rdi;
  LODWORD(rax) = rcx + rdx;
  var_5C = LODWORD(rax);
  var_2C = 0x0;
  var_30 = 0x0;
  if (LODWORD(rax) > LODWORD(rdx)) {
    var_78 = LODWORD(!LODWORD(rcx)) - LODWORD(rdx);
    r14 = var_2C;
    rbx = var_30;
    do {
      r15 = rdi;
      var_3C = LODWORD(getBytes(rdi, 0x5, 0x0, r14, rbx, r9));
      rax = getBytes(r15, 0x5, 0x0, r14, rbx, r9);
      LODWORD(rsi) = var_5C;
      rdx = rdx;
      LODWORD(rcx) = LODWORD(rax);
      var_38 = LODWORD(LODWORD(0x1) << LOBYTE(rcx));
      rdi = r9;
      LODWORD(r13) = LODWORD(rsi) - LODWORD(rdx);
      LODWORD(rax) = 0x10;
      if (LODWORD(r13) >= 0x11) {
	LODWORD(rax) = 0x10;
	LODWORD(r13) = LODWORD(rax);
      }
      if (LODWORD(r13) > 0x0) {
	var_38 = var_38 >> 0x1;
	LODWORD(rsi) = var_78 + rdx;
	if (LODWORD(rsi) <= 0xffffffee) {
	  LODWORD(rsi) = LODWORD(0xffffffef);
	}
	var_6C = LODWORD(rsi);
	rbx = rdx;
	var_68 = rbx;
	r12 = var_30;
	do {
	  rax = getBytes(var_50, var_3C, 0x1, r14, r12, var_48);
	  LODWORD(rcx) = LODWORD(rcx);
	  rsi = sign_extend_64(LODWORD(rbx));
	  LODWORD(rbx) = LODWORD(rbx) + 0x1;
	  *(int32_t *)(*(var_58 + 0x20) + rsi * 0x4) = LODWORD(LODWORD(LODWORD(rax) << LOBYTE(rcx)) + var_38);
	  LODWORD(r13) = LODWORD(r13) - 0x1;
	} while (LODWORD(r13) > 0x0);
	LODWORD(rax) = LODWORD(LODWORD(var_68) - 0x1) - var_6C;
	r9 = var_48;
	rdx = rax;
	rdi = var_50;
	LODWORD(rsi) = var_5C;
      }
      else {
	r9 = rdi;
	rdi = r15;
      }
      rbx = var_30;
    } while (LODWORD(rsi) > LODWORD(rdx));
  }
  return rax;
}

int decodeMira(_object*, _object*, _object*)(void * arg0, void * arg1, void * arg2) {
  PyFI::FuncIF::FuncIF(var_D8, arg1);
  var_E0 = 0x0;
  void PyFI::FuncIF::PosArg<PyFI::Array<unsigned char> >(var_D8, var_E0);
  var_E8 = 0x0;
  void PyFI::FuncIF::PosArg<PyFI::Array<long long> >(var_D8, var_E8);
  var_F0 = 0x0;
  void PyFI::FuncIF::PosArg<long long>(var_D8, var_F0);
  var_F8 = 0x0;
  r12 = var_E8;
  r15 = *(r12 + 0x10);
  rax = *r12;
  LODWORD(rdi) = 0x1;
  if (rax != 0x0) {
    rcx = *(r12 + 0x8);
    LODWORD(rbx) = 0x0;
    rdx = rax & 0xfffffffffffffffe;
    LODWORD(rdi) = 0x1;
    LODWORD(rsi) = 0x1;
    if (rdx != 0x0) {
      LODWORD(rdi) = 0x1;
      LODWORD(rbx) = 0x0;
      LODWORD(rsi) = 0x1;
      do {
	rdi = rdi * *(rcx + rbx * 0x8);
	rsi = rsi * *(rcx + rbx * 0x8 + 0x8);
	rbx = rbx + 0x2;
      } while (rdx != rbx);
      rbx = rdx;
    }
    rdi = rdi * rsi;
    rax = rax - rbx;
    if (rax != 0x0) {
      rcx = rcx + rbx * 0x8;
      do {
	rdi = rdi * *rcx;
	rcx = rcx + 0x8;
	rax = rax - 0x1;
      } while (rax != 0x0);
    }
  }
  r14 = calloc(rdi, 0x8);
  if (r15 != 0x0) {
    memcpy(r14, *(r12 + 0x20), r15 * 0x8);
  }
  var_108 = r15;
  rbx = malloc(r15 << 0x3);
  var_100 = rbx;
  memcpy(rbx, r14, r15 << 0x3);
  void PyFI::FuncIF::SetOutput<PyFI::Array<int> >(var_D8, var_F8, var_108);
  free(rbx);
  free(r14);
  var_10C = 0x0;
  r12 = var_F0;
  if (*r12 > 0x0) {
    r13 = var_E0;
    LODWORD(rcx) = 0x0;
    r14 = var_F8;
    r15 = var_10C;
    LODWORD(rbx) = 0x0;
    do {
      rdx = *(r13 + 0x20);
      LODWORD(rsi) = *(int8_t *)(rdx + sign_extend_32(LODWORD(rcx + 0x1))) & 0xff;
      LODWORD(rax) = *(int8_t *)(rdx + sign_extend_64(LODWORD(rcx))) & 0xff;
      LODWORD(rax) = LODWORD(rax) | LODWORD(LODWORD(rsi) << 0x8);
      LODWORD(rdi) = *(int8_t *)(rdx + sign_extend_64(LODWORD(sign_extend_64(LODWORD(rcx)) + 0x7))) & 0xff;
      LODWORD(rsi) = *(int8_t *)(rdx + sign_extend_64(LODWORD(sign_extend_64(LODWORD(rcx)) + 0x6))) & 0xff;
      LODWORD(rsi) = LODWORD(LODWORD(rsi) << 0x10) | LODWORD(LODWORD(rdi) << 0x18);
      LODWORD(rdi) = *(int8_t *)(rdx + sign_extend_64(LODWORD(sign_extend_64(LODWORD(rcx)) + 0x5))) & 0xff;
      LODWORD(rdi) = LODWORD(LODWORD(rdi) << 0x8) | LODWORD(rsi);
      LODWORD(rsi) = *(int8_t *)(rdx + sign_extend_64(LODWORD(sign_extend_64(LODWORD(rcx)) + 0x4))) & 0xff;
      var_10C = LODWORD(LODWORD(sign_extend_64(LODWORD(rcx))) + 0x8);
      LODWORD(rbx) = LODWORD(rbx) + LODWORD(sign_extend_16(LOWORD(rax)));
      decode(r13, r14, SAR(LODWORD(LODWORD(LODWORD(SAR(LODWORD(LODWORD(rsi) | LODWORD(rdi)), 0x1f)) >> 0x1e) + LODWORD(LODWORD(rsi) | LODWORD(rdi))), 0x2), SAR(LODWORD(LODWORD(LODWORD(SAR(LODWORD(sign_extend_16(LOWORD(rax))), 0x1f)) >> 0x1e) + LODWORD(sign_extend_16(LOWORD(rax)))), 0x2), r15);
      if (sign_extend_64(LODWORD(rbx)) >= *r12) {
	break;
      }
      LODWORD(rcx) = var_10C;
    } while (true);
  }
  rbx = PyFI::FuncIF::Output();
  PyFI::FuncIF::~FuncIF();
  rax = rbx;
  return rax;
}
