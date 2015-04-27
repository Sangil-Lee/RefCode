#include <stdio.h>                                                                                                                      
#include <string.h>                                                                                                                     
#include <unistd.h>                                                                                                                     
#include <libmemcached/memcached.h>                                                                                                     

/*
   *for this test
   in the 172.18.54.136 server,
   /usr/local/bin/memcached -d -m 128 -l 172.18.54.136 -p 11211
*/
int main(int argc, char *argv[])                                                                                                        
{                                                                                                                                       
  memcached_server_st *servers = NULL;                                                                                                  
  memcached_st *memc;                                                                                                                   
  memcached_return rc;                                                                                                                  
  char *key = "PVCCC";                                                                                                              
  char *value = "45.678";                                                                                                         
  char *retvalue = NULL;                                                                                                                
  size_t retlength;                                                                                                                     
  uint32_t flags;                                                                                                                       
                                                                                                                                        
  /* Create an empty memcached interface */                                                                                             
  memc = memcached_create(NULL);                                                                                                        
                                                                                                                                        
  /* Append a server to the list */                                                                                                     
  servers = memcached_server_list_append(servers, "172.18.54.136", 11211, &rc);                                                           
                                                                                                                                        
  /* Update the memcached structure with the updated server list */                                                                     
  rc = memcached_server_push(memc, servers);                                                                                            
                                                                                                                                        
  if (rc == MEMCACHED_SUCCESS)                                                                                                          
    fprintf(stderr,"Successfully added server\n");                                                                                      
  else                                                                                                                                  
    fprintf(stderr,"Couldn't add server: %s\n",memcached_strerror(memc, rc));                                                           
                                                                                                                                        
#if 0
  /* Store a value, without a timelimit */                                                                                              
  rc = memcached_set(memc, key, strlen(key), value, strlen(value), (time_t)0, (uint32_t)0);                                             
                                                                                                                                        
  if (rc == MEMCACHED_SUCCESS)                                                                                                          
    fprintf(stderr,"Value stored successfully\n");                                                                                      
  else                                                                                                                                  
    fprintf(stderr,"Couldn't store key: %s\n",memcached_strerror(memc, rc));                                                            
#endif
                                                                                                                                        
#if 0
  /* Retrieve the Stored value */                                                                                                       
  retvalue = memcached_get(memc, key, strlen(key), &retlength, &flags, &rc);                                                            
                                                                                                                                        
  if (rc == MEMCACHED_SUCCESS)                                                                                                          
    fprintf(stderr,"Value retrieved successfully: %s\n",retvalue);                                                                      
  else                                                                                                                                  
    fprintf(stderr,"Couldn't store key: %s\n",memcached_strerror(memc, rc));                                                            

#else
  memcached_return_t m_rc;
  char *keys[] = {"TEST_sinA", "TEST_sinB", "TEST_sinC"};
  size_t key_length[] = {9,9,9};
  unsigned int x;
  uint32_t mflags;

  char return_key[MEMCACHED_MAX_KEY];
  size_t return_key_length;
  char *return_value;
  size_t return_value_length;



  while(true)
  {
	  double dval;
	  x = 0;
	  m_rc = memcached_mget(memc, keys, key_length, 3);
	  while((return_value = memcached_fetch(memc, return_key, &return_key_length, &return_value_length, &mflags, &m_rc)))
	  {
		  memcpy(&dval, return_value, return_value_length);
		  printf("Key[%s], Value[%f]\n", keys[x], dval);
		  //free(return_value);
		  x++;
	  }
	  sleep(1);
  }

#endif
                                                                                                                                        
  return 0;                                                                                                                             
}
