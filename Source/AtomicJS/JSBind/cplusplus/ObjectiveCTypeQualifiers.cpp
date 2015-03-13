
#include "ObjectiveCTypeQualifiers.h"

using namespace CPlusPlus;

static inline int classify2(const char *s) {
  if (s[0] == 'i') {
    if (s[1] == 'n') {
      return Token_in;
    }
  }
  return Token_identifier;
}

static inline int classify3(const char *s) {
  if (s[0] == 'o') {
    if (s[1] == 'u') {
      if (s[2] == 't') {
        return Token_out;
      }
    }
  }
  return Token_identifier;
}

static inline int classify4(const char *s) {
    if (s[0] == 'c') {
        if (s[1] == 'o') {
            if (s[2] == 'p') {
                if (s[3] == 'y') {
                    return Token_copy;
                }
            }
        }
    }
    return Token_identifier;
}

static inline int classify5(const char *s) {
  if (s[0] == 'b') {
    if (s[1] == 'y') {
      if (s[2] == 'r') {
        if (s[3] == 'e') {
          if (s[4] == 'f') {
            return Token_byref;
          }
        }
      }
    }
  }
  else if (s[0] == 'i') {
    if (s[1] == 'n') {
      if (s[2] == 'o') {
        if (s[3] == 'u') {
          if (s[4] == 't') {
            return Token_inout;
          }
        }
      }
    }
  }
  return Token_identifier;
}

static inline int classify6(const char *s) {
  if (s[0] == 'a') {
      if (s[1] == 's') {
          if (s[2] == 's') {
              if (s[3] == 'i') {
                  if (s[4] == 'g') {
                      if (s[5] == 'n') {
                          return Token_assign;
                      }
                  }
              }
          }
      }
  }
  else if (s[0] == 'b') {
    if (s[1] == 'y') {
      if (s[2] == 'c') {
        if (s[3] == 'o') {
          if (s[4] == 'p') {
            if (s[5] == 'y') {
              return Token_bycopy;
            }
          }
        }
      }
    }
  }
  else if (s[0] == 'g') {
      if (s[1] == 'e') {
          if (s[2] == 't') {
              if (s[3] == 't') {
                  if (s[4] == 'e') {
                      if (s[5] == 'r') {
                          return Token_getter;
                      }
                  }
              }
          }
      }
  }
  else if (s[0] == 's') {
      if (s[1] == 'e') {
          if (s[2] == 't') {
              if (s[3] == 't') {
                  if (s[4] == 'e') {
                      if (s[5] == 'r') {
                          return Token_setter;
                      }
                  }
              }
          }
      }
  }
  else if (s[0] == 'o') {
    if (s[1] == 'n') {
      if (s[2] == 'e') {
        if (s[3] == 'w') {
          if (s[4] == 'a') {
            if (s[5] == 'y') {
              return Token_oneway;
            }
          }
        }
      }
    }
  }
  else if (s[0] == 'r') {
      if (s[1] == 'e') {
          if (s[2] == 't') {
              if (s[3] == 'a') {
                  if (s[4] == 'i') {
                      if (s[5] == 'n') {
                          return Token_retain;
                      }
                  }
              }
          }
      }
  }
  return Token_identifier;
}

static inline int classify8(const char *s) {
    if (s[0] == 'r') {
        if (s[1] == 'e') {
            if (s[2] == 'a') {
                if (s[3] == 'd') {
                    if (s[4] == 'o') {
                        if (s[5] == 'n') {
                            if (s[6] == 'l') {
                                if (s[7] == 'y') {
                                    return Token_readonly;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return Token_identifier;
}

static inline int classify9(const char *s) {
    if (s[0] == 'n') {
        if (s[1] == 'o') {
            if (s[2] == 'n') {
                if (s[3] == 'a') {
                    if (s[4] == 't') {
                        if (s[5] == 'o') {
                            if (s[6] == 'm') {
                                if (s[7] == 'i') {
                                    if (s[8] == 'c') {
                                        return Token_nonatomic;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    } else if (s[0] == 'r') {
        if (s[1] == 'e') {
            if (s[2] == 'a') {
                if (s[3] == 'd') {
                    if (s[4] == 'w') {
                        if (s[5] == 'r') {
                            if (s[6] == 'i') {
                                if (s[7] == 't') {
                                    if (s[8] == 'e') {
                                        return Token_readwrite;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return Token_identifier;
}

int CPlusPlus::classifyObjectiveCContextKeyword(const char *s, int n) {
  switch (n) {
    case 2: return classify2(s);
    case 3: return classify3(s);
    case 4: return classify4(s);
    case 5: return classify5(s);
    case 6: return classify6(s);
    case 8: return classify8(s);
    case 9: return classify9(s);
    default: return Token_identifier;
  } // switch
}


