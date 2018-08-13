/*
 * POSIX-compatible main layer
 *
 * Samuel Thibault <Samuel.Thibault@eu.citrix.net>, October 2007
 */

#ifdef HAVE_LIBC
#include <os.h>
#include <sched.h>
#include <console.h>
#include <netfront.h>
#include <pcifront.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <xenbus.h>
#include <events.h>

extern int main(int argc, char *argv[], char *envp[]);
extern void __libc_init_array(void);
extern void __libc_fini_array(void);
extern unsigned long __CTOR_LIST__[];
extern unsigned long __DTOR_LIST__[];

#if 0
#include <stdio.h>
int main(int argc, char *argv[], char *envp[])
{
    printf("Hello, World!\n");
    return 1;
}
#endif

void _init(void)
{
}

void _fini(void)
{
}

int x[2] = {1, 2};
int y[2] = {3, 4};
int z[2];

extern char __app_bss_start, __app_bss_end;
static void call_main(void *p)
{
    addvec(x, y, z, 2);gsdf
    printk("z = [%d %d]\n", z[0], z[1]);
    char *c, quote;
#ifdef CONFIG_QEMU_XS_ARGS
    char *domargs, *msg;
#endif
    int argc;
    char **argv;
    char *envp[] = { NULL };
#ifdef CONFIG_QEMU_XS_ARGS
    char *vm;
    char path[128];
    int domid;
#endif
    int i;

    /* Let other parts initialize (including console output) before maybe
     * crashing. */
    //sleep(1);

#ifdef CONFIG_SPARSE_BSS
    sparse((unsigned long) &__app_bss_start, &__app_bss_end - &__app_bss_start);
#endif
#if defined(HAVE_LWIP) && defined(CONFIG_START_NETWORK) && defined(CONFIG_NETFRONT)
    start_networking();
#endif
#ifdef CONFIG_PCIFRONT
    create_thread("pcifront", pcifront_watches, NULL);
#endif

#ifdef CONFIG_QEMU_XS_ARGS
    /* Fetch argc, argv from XenStore */
    domid = xenbus_read_integer("target");
    if (domid == -1) {
        printk("Couldn't read target\n");
        do_exit();
    }

    snprintf(path, sizeof(path), "/local/domain/%d/vm", domid);
    msg = xenbus_read(XBT_NIL, path, &vm);
    if (msg) {
        printk("Couldn't read vm path\n");
        do_exit();
    }
    printk("dom vm is at %s\n", vm);

    snprintf(path, sizeof(path), "%s/image/dmargs", vm);
    free(vm);
    msg = xenbus_read(XBT_NIL, path, &domargs);

    if (msg) {
        printk("Couldn't get stubdom args: %s\n", msg);
        domargs = strdup("");
    }
#endif

    argc = 1;

#define PARSE_ARGS(ARGS,START,QUOTE,END) \
    c = ARGS; \
    quote = 0; \
    while (*c) { \
	if (*c != ' ') { \
	    START; \
	    while (*c) { \
		if (quote) { \
		    if (*c == quote) { \
			quote = 0; \
			QUOTE; \
			continue; \
		    } \
		} else if (*c == ' ') \
		    break; \
		if (*c == '"' || *c == '\'') { \
		    quote = *c; \
		    QUOTE; \
		    continue; \
		} \
		c++; \
	    } \
	} else { \
            END; \
	    while (*c == ' ') \
		c++; \
	} \
    } \
    if (quote) {\
	printk("Warning: unterminated quotation %c\n", quote); \
	quote = 0; \
    }
#define PARSE_ARGS_COUNT(ARGS) PARSE_ARGS(ARGS, argc++, c++, )
#define PARSE_ARGS_STORE(ARGS) PARSE_ARGS(ARGS, argv[argc++] = c, memmove(c, c + 1, strlen(c + 1) + 1), *c++ = 0)

    PARSE_ARGS_COUNT((char*)start_info.cmd_line);
#ifdef CONFIG_QEMU_XS_ARGS
    PARSE_ARGS_COUNT(domargs);
#endif

    argv = alloca((argc + 1) * sizeof(char *));
    argv[0] = "main";
    argc = 1;

    PARSE_ARGS_STORE((char*)start_info.cmd_line)
#ifdef CONFIG_QEMU_XS_ARGS
    PARSE_ARGS_STORE(domargs)
#endif

    argv[argc] = NULL;

    for (i = 0; i < argc; i++)
	printf("\"%s\" ", argv[i]);
    printf("\n");

    __libc_init_array();
    environ = envp;
    for (i = 0; __CTOR_LIST__[i] != 0; i++)
        ((void((*)(void)))__CTOR_LIST__[i]) ();
    tzset();

    exit(main(argc, argv, envp));
}

void _exit(int ret)
{
    int i;

    for (i = 0; __DTOR_LIST__[i] != 0; i++)
        ((void((*)(void)))__DTOR_LIST__[i]) ();
    close_all_files();
    __libc_fini_array();
    printk("main returned %d\n", ret);
#if defined(HAVE_LWIP) && defined(CONFIG_NETFRONT)
    stop_networking();
#endif
    stop_kernel();
    if (!ret) {
	/* No problem, just shutdown.  */
        struct sched_shutdown sched_shutdown = { .reason = SHUTDOWN_poweroff };
        HYPERVISOR_sched_op(SCHEDOP_shutdown, &sched_shutdown);
    }
    do_exit();
}

int htoi(char s[])
{
    int n = 0;
    int i = 0;
    while (s[i] != '\0' && s[i] != '\n') {
        if (s[i] == '0') {
            if (s[i+1] == 'x' || s[i+1] == 'X')
                            i+=2;
        }
        if (s[i] >= '0' && s[i] <= '9') {
            n = n * 16 + (s[i] - '0');
        } else if (s[i] >= 'a' && s[i] <= 'f') {
            n = n * 16 + (s[i] - 'a') + 10;
        } else if (s[i] >= 'A' && s[i] <= 'F') {
            n = n * 16 + (s[i] - 'A') + 10;
        } else
            return -1;
        ++i;
    }
    return n;
}
unsigned long *gots;
unsigned long *gotm = (unsigned long *)(0x9a540);
struct timeval start, end;

void update_minios_parse_sym(void)
{
   /*write moduleID to dom0*/
   int domid;
   char path[128];
   char value[10];
   char *msg, *realaddr;
   int i;

   gettimeofday(&start, NULL);
   domid = xenbus_read_integer("domid");
   snprintf(path, sizeof(path), "/local/domain/%d/console/mid", domid);
   sprintf(value, "%p", gotm[1]);
   xenbus_write(0, path, value);
   gettimeofday(&end, NULL);

   printk("--------parse1 dym: %ldus\n", 1000000 * ( end.tv_sec - start.tv_sec ) + end.tv_usec -start.tv_usec);
   //gotm[3] = (unsigned long)0xdf1a0; 
   //getchar();
   gettimeofday(&start, NULL);
   for (i=0; i<3; i++){
       snprintf(path, sizeof(path), "/local/domain/%d/console/realaddr%d", domid, i);
       msg = xenbus_read(XBT_NIL, path, &realaddr);
       if (msg)
           printk("Could not read realaddr!\n");
       gotm[3+i] = (unsigned long)(0xfd000+htoi(realaddr));
   }
}

void update_sotest_parse_sym(void)
{
   int domid, count1, i;   
   char path[128];
   char *msg, *val_count, *realaddr;

   domid = xenbus_read_integer("domid");
   snprintf(path, sizeof(path), "/local/domain/%d/console/count1", domid);
   msg = xenbus_read(XBT_NIL, path, &val_count);
   count1 = atoi(val_count);

   for (i=0; i<count1; i++){
       snprintf(path, sizeof(path), "/local/domain/%d/console/addr1_%d", domid, i);
       msg = xenbus_read(XBT_NIL, path, &realaddr);
       if (msg)
           printk("Could not read realaddr!\n");
       gots[3+i] = (unsigned long )htoi(realaddr);
   }
   //printf("-------realaddr = %s\n", realaddr);
   gettimeofday(&end, NULL);
   printk("--------rel2 dym: %ldus\n", 1000000 * ( end.tv_sec - start.tv_sec ) + end.tv_usec -start.tv_usec);
}

/*get updated shared library*/
static void get_shared_library(void *p)
{
   /*
   int domid;
   char pathdomgfn[128];
   unsigned long dom_gfn;
   char gfn[64];
   char *shared_addr;
*/
   /*step 1: get a pfn of page*/
/* shared_addr = (char *)alloc_page();
   dom_gfn = virt_to_pfn(shared_addr);
   snprintf(gfn, sizeof(gfn), "%d", dom_gfn);
*/
   /*step 2: write pfn to xenstore*/
   /*
   domid = xenbus_read_integer("domid");
   snprintf(pathdomgfn, sizeof(pathdomgfn), "/local/domain/%d/console/domgfn", domid);
   xenbus_write(0, pathdomgfn, gfn);
   memcpy(shared_addr, "Hello World", sizeof("Hello World"));
   printf("shared_addr = %s\n", shared_addr);
   */

   /*
   int server_sockfd;
   int client_sockfd;
   int len;
   struct sockaddr_in my_addr; //server
   struct sockaddr_in remote_addr; //client
   int sin_size = sizeof(struct sockaddr_in);

   //declare a struct of socket
   memset(&my_addr, 0, sizeof(my_addr));//init sockaddr_in
   my_addr.sin_family = AF_INET; //Set IP communication
   my_addr.sin_addr.s_addr = INADDR_ANY;//server IP, permit any IP to connect
   my_addr.sin_port = htons(10000);//set a server port

   //build a socke, if success, return a fd of socket
   if ((server_sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0){
       perror("socket");
       return 1;
   }

   //bind socket to network IP
   if (bind(server_sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) < 0){
       perror("bind");
       return 1;
   }

   // listen a request
   listen(server_sockfd, 5);
   
   //wait for a request
   if ((client_sockfd = accept(server_sockfd, (struct sockaddr *)&remote_addr, &sin_size)) < 0){
       perror("accept");
       return 1;
   }
   char *buf = (char *)malloc(sizeof(char) *512);
   char **dest = (char **)malloc(sizeof(char *)*2);
   int i;
   for (i=0; i<2; i++)
       dest[i] = (char *)malloc(sizeof(char)*512);
   i=0;
   while ((len = recv(client_sockfd, buf, 512, 0)) > 0){
       memcpy(dest[i], buf, 512);
       i++;
   }
   for (i=0; i<512; i++)
       printf("%p, ", dest[0][i]);
   printf("\n\n\n");
   close(client_sockfd);
   close(server_sockfd);
   return 0;
   */
   /*get .got.plt of libso_test2.so*/
   int domid;
   char path[128];
   char *msg, *val;

   domid = xenbus_read_integer("domid");
   snprintf(path, sizeof(path), "/local/domain/%d/console/got", domid);
   while (1){
       msg = xenbus_read(XBT_NIL, path, &val); 
       if (msg)
           continue;
       if (gots == htoi(val)+0xfd000)
           continue;
       //printf("-----val = %s\n", val);
       gettimeofday(&start, NULL);
       gots = (unsigned long *)(htoi(val)+0xfd000);
       //printk("------------gots[0] = %p\n", gots[0]);
       /*set moduleID in gots[1]*/
       gots[1] = (unsigned long)0x1;       
       /*set got[3] to update_parse_sym*/
       gotm[3] = update_minios_parse_sym;
       gots[3] = update_sotest_parse_sym;
       gettimeofday(&end, NULL);
       printk("------resolve2 time : %ldus\n", 1000000*(end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec);
   }

}


/*parse symbol, like _dl_runtime_resolve*/
void lazy_binding_parse_sym()
{
   int domid, count0, count1, i, j;
   char path[128];
   //char value[12];
   //unsigned long *got = (unsigned long *)(&_edata+0xa0);
   char *msg, *val_count, *realaddr;
   unsigned long *got0, *got1;
   
   /*step 1:write addr into xenstore!*/
   domid = xenbus_read_integer("domid");
   snprintf(path, sizeof(path), "/local/domain/%d/console/flag", domid);
   //sprintf(value, "%p", got[1]);
   xenbus_write(0, path, "1");
   //printk("value = %s\n", value);
   /*step 2:read xenstore and set .got.plt table*/
   //label 1: read count
   snprintf(path, sizeof(path), "/local/domain/%d/console/count0", domid);
   while (1) {
       msg = xenbus_read(XBT_NIL, path, &val_count);
       if (msg)
           continue;
       break;
   }
   count0 = atoi(val_count);
   //printf("count = %d\n", count);
   //printf("val_count = %s\n", val_count);
   //label 2: read addr
   //find .plt.got
   //printf("----------------count = %d\n", count);
   snprintf(path, sizeof(path), "/local/domain/%d/console/count1", domid);
   msg = xenbus_read(XBT_NIL, path, &val_count);
   count1 = atoi(val_count);
   
   for (j=0; j<2; j++){
       snprintf(path, sizeof(path), "/local/domain/%d/console/r_got%d", domid, j);
       if (j == 0){
           msg = xenbus_read(XBT_NIL, path, &realaddr);
           if (msg)
               printk("Could not read realaddr1!\n");
           got0 = (unsigned long *)htoi(realaddr);
           //printf("-------------got0 = %p\n", got0);
       }
       if (j == 1){
           msg = xenbus_read(XBT_NIL, path, &realaddr);
           if (msg)
               printk("Could not read realaddr2!\n");
           got1 = (unsigned long *)(htoi(realaddr)+0xdd000);
           //printf("-------------got1 = %p\n", got1);
       }
   }
   
   for (i=0; i<count0; i++){
       snprintf(path, sizeof(path), "/local/domain/%d/console/addr0_%d", domid, i);
       msg = xenbus_read(XBT_NIL, path, &realaddr);
       if (msg)
           printk("Could not read realaddr3!\n");
       //printf("realaddr = %p\n", htoi(realaddr));
       got0[i+3] = (unsigned long)htoi(realaddr);
   }
   for (i=0; i<count1; i++){
       snprintf(path, sizeof(path), "/local/domain/%d/console/addr1_%d", domid, i);
       msg = xenbus_read(XBT_NIL, path, &realaddr);
       if (msg)
           printk("Could not read realaddr4!\n");
       //printf("realaddr = %p\n", htoi(realaddr));
       got1[i+3] = (unsigned long)htoi(realaddr);
   }
   //__asm __volatile ("call %0" :: "m" (got0[3]));
}


/*define plt_func*/
/*
void plt_func(void)
{
   __asm __volatile ("call lazy_binding_parse_sym");
}
*/

/*init .plt.got, make it to point plt_func*/
/*
void init_plt_got(unsigned long *got)
{
   got[3] = (unsigned long)plt_func;
}
*/

int app_main(start_info_t *si)
{
   /*step 1: init .plt.got*/
   unsigned long *got = (unsigned long *)(0x9a540);
// unsigned long *gotl = (unsigned long *)(0xde2b0);

/* 
   unsigned long *got1 = (unsigned long *)0xce2b0;
   got0[3] = (unsigned long)0xcd1a0;
*/
   //got[3] = (unsigned long)0xdd1a0;
   got[3] = lazy_binding_parse_sym;
// gotl[3] = lazy_binding_parse_sym;
// lazy_binding_parse_sym();
// lazy_thread = create_thread("lazy", lazy_binding_parse_sym, NULL);
// sleep(10);
// init_plt_got(got);
   /*step 2: */
   //printk("--------got[1] = %p\n", got[1]);
   /*update shared library!*/


    printk("Dummy main: start_info=%p\n", si);
    main_thread = create_thread("main", call_main, si);
    update_thread = create_thread("update", get_shared_library, NULL);

    return 0;
}
#endif
