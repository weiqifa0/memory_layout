
static void get_pgtable_macro(void)
{
    printk("PAGE_OFFSET = 0x%lx\n", PAGE_OFFSET);
    printk("PGDIR_SHIFT = %d\n", PGDIR_SHIFT);
    printk("PUD_SHIFT = %d\n", PUD_SHIFT);
    printk("PMD_SHIFT = %d\n", PMD_SHIFT);
    printk("PAGE_SHIFT = %d\n", PAGE_SHIFT);

    printk("PTRS_PER_PGD = %d\n", PTRS_PER_PGD);
    printk("PTRS_PER_PUD = %d\n", PTRS_PER_PUD);
    printk("PTRS_PER_PMD = %d\n", PTRS_PER_PMD);
    printk("PTRS_PER_PTE = %d\n", PTRS_PER_PTE);

    printk("PAGE_MASK = 0x%lx\n", PAGE_MASK);
}

static unsigned long vaddr2paddr(unsigned long vaddr)
{
    pgd_t *pgd;
    pud_t *pud;
    pmd_t *pmd;
    pte_t *pte;
    unsigned long paddr = 0;
        unsigned long page_addr = 0;
    unsigned long page_offset = 0;

    pgd = pgd_offset(current->mm, vaddr);
    printk("pgd_val = 0x%lx\n", pgd_val(*pgd));
    printk("pgd_index = %lu\n", pgd_index(vaddr));
    if (pgd_none(*pgd)) {
        printk("not mapped in pgd\n");
        return -1;
    }

    pud = pud_offset(pgd, vaddr);
    printk("pud_val = 0x%lx\n", pud_val(*pud));
    if (pud_none(*pud)) {
        printk("not mapped in pud\n");
        return -1;
    }

    pmd = pmd_offset(pud, vaddr);
    printk("pmd_val = 0x%lx\n", pmd_val(*pmd));
    printk("pmd_index = %lu\n", pmd_index(vaddr));
    if (pmd_none(*pmd)) {
        printk("not mapped in pmd\n");
        return -1;
    }

    pte = pte_offset_kernel(pmd, vaddr);
    printk("pte_val = 0x%lx\n", pte_val(*pte));
    printk("pte_index = %lu\n", pte_index(vaddr));
    if (pte_none(*pte)) {
        printk("not mapped in pte\n");
        return -1;
    }

    /* Page frame physical address mechanism | offset */
    page_addr = pte_val(*pte) & PAGE_MASK;
    page_offset = vaddr & ~PAGE_MASK;
    paddr = page_addr | page_offset;
    printk("page_addr = %lx, page_offset = %lx\n", page_addr, page_offset);
        printk("vaddr = %lx, paddr = %lx\n", vaddr, paddr);

    return paddr;
}

static int __init v2p_init(void)
{
    unsigned long vaddr = 0;

    printk("vaddr to paddr module is running..\n");
    get_pgtable_macro();
    printk("\n");

    vaddr = (unsigned long)vmalloc(1000 * sizeof(char));
    if (vaddr == 0) {
        printk("vmalloc failed..\n");
        return 0;
    }
    printk("vmalloc_vaddr=0x%lx\n", vaddr);
    vaddr2paddr(vaddr);

    printk("\n\n");
    vaddr = __get_free_page(GFP_KERNEL);
    if (vaddr == 0) {
        printk("__get_free_page failed..\n");
        return 0;
    }
    printk("get_page_vaddr=0x%lx\n", vaddr);
    vaddr2paddr(vaddr);

    return 0;
}

static void __exit v2p_exit(void)
{
    printk("vaddr to paddr module is leaving..\n");
        vfree((void *)vaddr);
        free_page(vaddr);
}

#if 0
Get_pgtable_macro () Prints some macros in the current system paging mechanism.

Through vmalloc () in the allocation of memory space in kernel space, calling vaddr2paddr () will be converted into a virtual address physical address.

Use vaddr2paddr () to translate the virtual address into a physical address by allocating the frame in kernel space with __get_free_pages ().
Release the requested memory space through vfree () and free_page (), respectively.
Vaddr2paddr () is executed as follows:

Calculate the linear address pgd of the page global catalog entry by pgd_offset, passing in the memory descriptor mm and the linear address vaddr. Next, print the page global catalog entry pointed to by pgd.

Calculate the linear address pud of the page parent directory entry by pud_offset, passing the parameters to the linear address pgd of the page global directory entry and the linear address vaddr. Then print the pud referred to the parent directory entry.

Calculate the linear address pmd of the page middle directory entry through pmd_offset, passing the parameters to the linear address pud and the linear address vaddr of the parent directory entry. Then print the middle of the page referred to pmd directory entries.

Pte_offset_kernel pte_offset_kernel calculated by the linear address pte, the parameters for the middle of the directory entry of the linear address pmd linear address and address vaddr. Then print the page table item pointed to by pte.

pte_val (* pte) to remove the page table entries, and PAGE_MASK phase and the result is to access the page's physical address; vaddr & ~ PAGE_MASK used to get linear address offset field; the two or the final physical address calculation.

Print the physical address

#endif
