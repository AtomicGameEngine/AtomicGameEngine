
#include "Lexer.h"
#include "Token.h"

using namespace CPlusPlus;

static inline int classify3(const char *s) {
  if (s[0] == 'e') {
    if (s[1] == 'n') {
      if (s[2] == 'd') {
        return T_AT_END;
      }
    }
  }
  else if (s[0] == 't') {
    if (s[1] == 'r') {
      if (s[2] == 'y') {
        return T_AT_TRY;
      }
    }
  }
  return T_ERROR;
}

static inline int classify4(const char *s) {
  if (s[0] == 'd') {
    if (s[1] == 'e') {
      if (s[2] == 'f') {
        if (s[3] == 's') {
          return T_AT_DEFS;
        }
      }
    }
  }
  return T_ERROR;
}

static inline int classify5(const char *s) {
  if (s[0] == 'c') {
    if (s[1] == 'a') {
      if (s[2] == 't') {
        if (s[3] == 'c') {
          if (s[4] == 'h') {
            return T_AT_CATCH;
          }
        }
      }
    }
    else if (s[1] == 'l') {
      if (s[2] == 'a') {
        if (s[3] == 's') {
          if (s[4] == 's') {
            return T_AT_CLASS;
          }
        }
      }
    }
  }
  else if (s[0] == 't') {
    if (s[1] == 'h') {
      if (s[2] == 'r') {
        if (s[3] == 'o') {
          if (s[4] == 'w') {
            return T_AT_THROW;
          }
        }
      }
    }
  }
  return T_ERROR;
}

static inline int classify6(const char *s) {
  if (s[0] == 'e') {
    if (s[1] == 'n') {
      if (s[2] == 'c') {
        if (s[3] == 'o') {
          if (s[4] == 'd') {
            if (s[5] == 'e') {
              return T_AT_ENCODE;
            }
          }
        }
      }
    }
  }
  else if (s[0] == 'p') {
    if (s[1] == 'u') {
      if (s[2] == 'b') {
        if (s[3] == 'l') {
          if (s[4] == 'i') {
            if (s[5] == 'c') {
              return T_AT_PUBLIC;
            }
          }
        }
      }
    }
  }
  return T_ERROR;
}

static inline int classify7(const char *s) {
  if (s[0] == 'd') {
    if (s[1] == 'y') {
      if (s[2] == 'n') {
        if (s[3] == 'a') {
          if (s[4] == 'm') {
            if (s[5] == 'i') {
              if (s[6] == 'c') {
                return T_AT_DYNAMIC;
              }
            }
          }
        }
      }
    }
  }
  else if (s[0] == 'f') {
    if (s[1] == 'i') {
      if (s[2] == 'n') {
        if (s[3] == 'a') {
          if (s[4] == 'l') {
            if (s[5] == 'l') {
              if (s[6] == 'y') {
                return T_AT_FINALLY;
              }
            }
          }
        }
      }
    }
  }
  else if (s[0] == 'p') {
    if (s[1] == 'a') {
      if (s[2] == 'c') {
        if (s[3] == 'k') {
          if (s[4] == 'a') {
            if (s[5] == 'g') {
              if (s[6] == 'e') {
                return T_AT_PACKAGE;
              }
            }
          }
        }
      }
    }
    else if (s[1] == 'r') {
      if (s[2] == 'i') {
        if (s[3] == 'v') {
          if (s[4] == 'a') {
            if (s[5] == 't') {
              if (s[6] == 'e') {
                return T_AT_PRIVATE;
              }
            }
          }
        }
      }
    }
  }
  return T_ERROR;
}

static inline int classify8(const char *s) {
  if (s[0] == 'o') {
    if (s[1] == 'p') {
      if (s[2] == 't') {
        if (s[3] == 'i') {
          if (s[4] == 'o') {
            if (s[5] == 'n') {
              if (s[6] == 'a') {
                if (s[7] == 'l') {
                  return T_AT_OPTIONAL;
                }
              }
            }
          }
        }
      }
    }
  }
  else if (s[0] == 'p') {
    if (s[1] == 'r') {
      if (s[2] == 'o') {
        if (s[3] == 'p') {
          if (s[4] == 'e') {
            if (s[5] == 'r') {
              if (s[6] == 't') {
                if (s[7] == 'y') {
                  return T_AT_PROPERTY;
                }
              }
            }
          }
        }
        else if (s[3] == 't') {
          if (s[4] == 'o') {
            if (s[5] == 'c') {
              if (s[6] == 'o') {
                if (s[7] == 'l') {
                  return T_AT_PROTOCOL;
                }
              }
            }
          }
        }
      }
    }
  }
  else if (s[0] == 'r') {
    if (s[1] == 'e') {
      if (s[2] == 'q') {
        if (s[3] == 'u') {
          if (s[4] == 'i') {
            if (s[5] == 'r') {
              if (s[6] == 'e') {
                if (s[7] == 'd') {
                  return T_AT_REQUIRED;
                }
              }
            }
          }
        }
      }
    }
  }
  else if (s[0] == 's') {
    if (s[1] == 'e') {
      if (s[2] == 'l') {
        if (s[3] == 'e') {
          if (s[4] == 'c') {
            if (s[5] == 't') {
              if (s[6] == 'o') {
                if (s[7] == 'r') {
                  return T_AT_SELECTOR;
                }
              }
            }
          }
        }
      }
    }
  }
  return T_ERROR;
}

static inline int classify9(const char *s) {
  if (s[0] == 'i') {
    if (s[1] == 'n') {
      if (s[2] == 't') {
        if (s[3] == 'e') {
          if (s[4] == 'r') {
            if (s[5] == 'f') {
              if (s[6] == 'a') {
                if (s[7] == 'c') {
                  if (s[8] == 'e') {
                    return T_AT_INTERFACE;
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  else if (s[0] == 'p') {
    if (s[1] == 'r') {
      if (s[2] == 'o') {
        if (s[3] == 't') {
          if (s[4] == 'e') {
            if (s[5] == 'c') {
              if (s[6] == 't') {
                if (s[7] == 'e') {
                  if (s[8] == 'd') {
                    return T_AT_PROTECTED;
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  return T_ERROR;
}

static inline int classify10(const char *s) {
  if (s[0] == 's') {
    if (s[1] == 'y') {
      if (s[2] == 'n') {
        if (s[3] == 't') {
          if (s[4] == 'h') {
            if (s[5] == 'e') {
              if (s[6] == 's') {
                if (s[7] == 'i') {
                  if (s[8] == 'z') {
                    if (s[9] == 'e') {
                      return T_AT_SYNTHESIZE;
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  return T_ERROR;
}

static inline int classify11(const char *s) {
  if (s[0] == 'n') {
    if (s[1] == 'o') {
      if (s[2] == 't') {
        if (s[3] == '_') {
          if (s[4] == 'k') {
            if (s[5] == 'e') {
              if (s[6] == 'y') {
                if (s[7] == 'w') {
                  if (s[8] == 'o') {
                    if (s[9] == 'r') {
                      if (s[10] == 'd') {
                        return T_AT_NOT_KEYWORD;
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  return T_ERROR;
}

static inline int classify12(const char *s) {
  if (s[0] == 's') {
    if (s[1] == 'y') {
      if (s[2] == 'n') {
        if (s[3] == 'c') {
          if (s[4] == 'h') {
            if (s[5] == 'r') {
              if (s[6] == 'o') {
                if (s[7] == 'n') {
                  if (s[8] == 'i') {
                    if (s[9] == 'z') {
                      if (s[10] == 'e') {
                        if (s[11] == 'd') {
                          return T_AT_SYNCHRONIZED;
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  return T_ERROR;
}

static inline int classify14(const char *s) {
  if (s[0] == 'i') {
    if (s[1] == 'm') {
      if (s[2] == 'p') {
        if (s[3] == 'l') {
          if (s[4] == 'e') {
            if (s[5] == 'm') {
              if (s[6] == 'e') {
                if (s[7] == 'n') {
                  if (s[8] == 't') {
                    if (s[9] == 'a') {
                      if (s[10] == 't') {
                        if (s[11] == 'i') {
                          if (s[12] == 'o') {
                            if (s[13] == 'n') {
                              return T_AT_IMPLEMENTATION;
                            }
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  return T_ERROR;
}

static inline int classify19(const char *s) {
  if (s[0] == 'c') {
    if (s[1] == 'o') {
      if (s[2] == 'm') {
        if (s[3] == 'p') {
          if (s[4] == 'a') {
            if (s[5] == 't') {
              if (s[6] == 'i') {
                if (s[7] == 'b') {
                  if (s[8] == 'i') {
                    if (s[9] == 'l') {
                      if (s[10] == 'i') {
                        if (s[11] == 't') {
                          if (s[12] == 'y') {
                            if (s[13] == '_') {
                              if (s[14] == 'a') {
                                if (s[15] == 'l') {
                                  if (s[16] == 'i') {
                                    if (s[17] == 'a') {
                                      if (s[18] == 's') {
                                        return T_AT_COMPATIBILITY_ALIAS;
                                      }
                                    }
                                  }
                                }
                              }
                            }
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  return T_ERROR;
}

int Lexer::classifyObjCAtKeyword(const char *s, int n) {
  switch (n) {
    case 3: return classify3(s);
    case 4: return classify4(s);
    case 5: return classify5(s);
    case 6: return classify6(s);
    case 7: return classify7(s);
    case 8: return classify8(s);
    case 9: return classify9(s);
    case 10: return classify10(s);
    case 11: return classify11(s);
    case 12: return classify12(s);
    case 14: return classify14(s);
    case 19: return classify19(s);
    default: return T_ERROR;
  } // switch
}
