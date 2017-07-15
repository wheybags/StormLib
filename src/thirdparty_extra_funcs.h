#ifndef THIRDPATY_EXTRA_FUNCS_H
#define THIRDPATY_EXTRA_FUNCS_H


// orignally added to tomcrypt_pk.h
/* Ladik: Added for verifying Blizzard strong signature verification */
extern "C" int rsa_verify_simple(const unsigned char *sig,  unsigned long siglen,
                      const unsigned char *hash, unsigned long hashlen,
                            int           *stat,
							rsa_key *key);

#endif
