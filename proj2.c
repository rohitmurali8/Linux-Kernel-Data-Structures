#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/hashtable.h>
#include <linux/radix-tree.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <asm/uaccess.h>
#include <linux/rbtree.h>

static char *int_str;

/* [X1: point 1]
 * Exposes symbols to Linux when we use the GPL license
 */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("ROHIT MURALI");
MODULE_DESCRIPTION("LKP PROJECT 2");

/* [X2: point 1]
 * Variables that will take values of command line arguments and pass them as arguments to this module
 */
module_param(int_str, charp, S_IRUSR | S_IRGRP | S_IROTH);

/* [X3: point 1]
 * Used to document the module parameters
 */
MODULE_PARM_DESC(int_str, "A comma-separated list of integers");

/* [X4: point 1]
 * Initializes the HEAD of a list 
 */
static LIST_HEAD(mylist);

/* [X5: point 1]
 * Defines the structure variables of the linked list.
 */
struct entry {
	int val;
	struct list_head list;
};

static DEFINE_HASHTABLE(tbl, 10);

struct h_node {
	int data;
	struct hlist_node node;
};

struct rb_tree{
	int data;
	struct rb_node node;
};

struct rb_root mytree = RB_ROOT;

RADIX_TREE(rt, GFP_KERNEL);

struct radix_tree{
	int data;
};

static int myhash(int key) {
    int index;
    index = key%sizeof(tbl);

    return index;
}

DEFINE_XARRAY(myxarray);

static int store_linked_list(int val)
{
	/* [X6: point 10]
	 * Allocate a struct entry of which val is val
	 * and add it to the tail of mylist.
	 * Return 0 if everything is successful.
	 * Otherwise (e.g., memory allocation failure),
	 * return corresponding error code in error.h (e.g., -ENOMEM).
	 */

	struct entry *my_entry;
	my_entry = kmalloc(sizeof(struct entry), GFP_KERNEL);
	if (my_entry == NULL){
		return -ENOMEM;
	}
	my_entry->val = val;
	INIT_LIST_HEAD(&my_entry->list);
	list_add_tail(&my_entry->list, &mylist);
	return 0;
}

static int store_hash_table(int val)	
{
	struct h_node *a;
	int key_a;
	a = kmalloc(sizeof(struct h_node), GFP_KERNEL);
	if(a == NULL)
	{
		return -ENOMEM;
	}
	a->data = val;
	key_a = myhash(a->data);
	hash_add(tbl, &a->node, key_a);
	return 0;
}

static int store_rb_tree(struct rb_root *root, int val)
{
	struct rb_tree *new;
	struct rb_node **link = &root->rb_node;
        struct rb_node *parent = NULL;
	new = kmalloc(sizeof(struct rb_tree), GFP_KERNEL);
	if (new == NULL)
	{
		return -ENOMEM;
	}
	new->data = val;
	//old->data = val;
	//value = old->data;
	while(*link)
	{
		parent = *link;
		//struct rb_tree *stuff = rb_entry(parent, struct rb_tree, parent);
		struct rb_tree *source = rb_entry(parent, struct rb_tree, node);
		if (source->data > val)
			link = &(*link)->rb_left;
		else
			link = &(*link)->rb_right;
	}

	rb_link_node(&new->node, parent, link);
	rb_insert_color(&new->node, root);
	return 0;

}

static int store_radix_tree(int val)
{
	//struct radix_tree *rt;
	//struct radix_tree_root *root;
	int *index;
	int ret;
	index = kmalloc(sizeof(int), GFP_KERNEL);
	if(index == NULL)
	{
		return -ENOMEM;
	}
	*index = val;
	ret = radix_tree_insert(&rt, (long)val, index);
	if (ret != 0){
		printk(KERN_INFO "%d", ret);
	}
	return 0;
}

static int store_xarray(int val)
{
	int *value;
	value = kmalloc(sizeof(int), GFP_KERNEL);
	if (value == NULL)
	{
		return -ENOMEM;
	}
	*value = val;
	xa_store(&myxarray, val, value, GFP_KERNEL);
	return 0;
}

static void test_linked_list(struct seq_file *m)
{
	/* [X7: point 10]
	 * Print out value of all entries in mylist.
	 */
	struct entry *enter;
	printk(KERN_INFO "Linked List: ");
	seq_printf(m, "Linked List: ");
	list_for_each_entry(enter, &mylist, list){
		printk(KERN_INFO " %d ", enter->val);
		seq_printf(m, "%d ", enter->val);
	}
	seq_printf(m, "\n");
}

static void hash_table_values(struct seq_file *m)
{
	struct h_node *curr;
	unsigned int bkt;
	printk(KERN_INFO "Hash Table: ");
	seq_printf(m, "Hash Table: ");
	hash_for_each(tbl, bkt, curr, node){
		printk(KERN_INFO "%d ", curr->data);
		seq_printf(m, "%d ", curr->data);
	}
	seq_printf(m, "\n");
}

static void rb_tree_values(struct seq_file *m)
{
	//struct rb_root *root;
	struct rb_node *n;
	struct rb_tree *ans;
	n = rb_first(&mytree);
	printk(KERN_INFO "RB Tree: ");
	seq_printf(m, "RB Tree: ");
	while (n != NULL)
	{
		ans = rb_entry(n, struct rb_tree, node);
		printk(KERN_INFO "%d ", ans->data);
		seq_printf(m, "%d ", ans->data);
		n = rb_next(n);
	}
	seq_printf(m, "\n");
}

static void radix_tree_values(struct seq_file *m)
{
	//struct radix_tree_root *root;
	int key = 1;
	int *value;
	printk(KERN_INFO "Radix Tree: ");
	seq_printf(m, "Radix Tree: ");
	while (radix_tree_lookup(&rt, key) != NULL)
	{
		value = radix_tree_lookup(&rt, (long)key);
		printk(KERN_INFO "%d ", *value);
		seq_printf(m, "%d ", *value);
		key++;
	}
	seq_printf(m, "\n");
}

static void xarray_values(struct seq_file *m)
{
	int key = 1;
	int *value;
	printk(KERN_INFO "Xarray: ");
	seq_printf(m, "Xarray: ");
	while (xa_load(&myxarray, key) != NULL)
	{
		value = xa_load(&myxarray, key);
		printk(KERN_INFO "%d ", *value);
		seq_printf(m, "%d ", *value);
		key++;
	}
	seq_printf(m, "\n");
}
static void destroy_linked_list_and_free(void)
{
	/* [X8: point 10]
	 * Free all entries in mylist.
	 */
	struct entry *ptr, *next;
	list_for_each_entry_safe(ptr, next, &mylist, list){
		//printk(KERN_INFO "Removing value from linked list %d \n", ptr->val);
		list_del(&ptr->list);
		kfree(ptr);
	}

}

static void destroy_hash_table_and_free(void)
{
	struct h_node *del;
	struct hlist_node *temp;
	unsigned bkt;
	hash_for_each_safe(tbl, bkt, temp, del, node){
		//printk(KERN_INFO "Removing value from hash table %d \n", del->data);
		hash_del(&del->node);
		kfree(del);
	}
}

static void destroy_rb_tree_and_free(void)
{
	struct rb_tree *tree;
	struct rb_node *n;
	n = rb_last(&mytree);
	while(n != NULL)
	{
		tree = rb_entry(n, struct rb_tree, node);
		n = rb_prev(n);
		rb_erase(&tree->node, &mytree);
		kfree(tree);
	}
}


static void destroy_radix_tree_and_free(void)
{
	int key = 1;
	int *value;
	while ((value = radix_tree_delete(&rt, key)) != NULL){
		//printk(KERN_INFO "Removing from radix tree %d \n", *value);
		key++;
	}
}

static void destroy_xarray_and_free(void)
{
	int key = 1;
	int *value;
	while ((value = xa_erase(&myxarray, key)) != NULL){
		//printk(KERN_INFO "Removing from Xarray %d \n", *value);
		key++;
	}
}
static int parse_params(void)
{
	int val, err = 0;
	char *p, *orig, *params;


	/* [X9: point 1]
	 * Allocates space for an existing string and then copies it into that space.
	 */
	params = kstrdup(int_str, GFP_KERNEL);
	if (!params)
		return -ENOMEM;
	orig = params;

	/* [X10: point 1]
	 * Checks whether the next token obtained from the string points to NULL or not
	 */
	while ((p = strsep(&params, ",")) != NULL) {
		if (!*p)
			continue;
		/* [X11: point 1]
		 * Converts the tokenized string ito an integer
		 */
		err = kstrtoint(p, 0, &val);
		if (err)
			break;

		/* [X12: point 1]
		 * Adds the converted integer value to the tail of the linked list data structure
		 */
		err = store_linked_list(val);
		if (err)
			break;

		err = store_hash_table(val);
		if (err)
			break;

		err = store_rb_tree(&mytree, val);
		if (err)
			break;
		
		err = store_radix_tree(val);
		if (err)
			break;

		err = store_xarray(val);
		if (err)
			break;
	}

	/* [X13: point 1]
	 * Function used  to free the originally allocated memory of the parameter
	 */
	kfree(orig);
	return err;
}

static void run_tests(struct seq_file *m)
{
	/* [X14: point 1]
	 * Prints out all the values in the linked list.
	 */
	test_linked_list(m);
	hash_table_values(m);
	rb_tree_values(m);
	radix_tree_values(m);
	xarray_values(m);
}

static void cleanup(void)
{
	/* [X15: point 1]
	 * Free all the entries inside the linked list
	 */
	printk(KERN_INFO "\nCleaning up...\n");

	destroy_linked_list_and_free();
	destroy_hash_table_and_free();
	destroy_rb_tree_and_free();
	destroy_radix_tree_and_free();
	destroy_xarray_and_free();
}

static int ex4_init(struct seq_file *m, void *v)
{
	int err = 0;

	/* [X16: point 1]
	 * Used to check if the user has entered the command line arguments to be used as module parameters.
	 */
	if (!int_str) {
		printk(KERN_INFO "Missing \'int_str\' parameter, exiting\n");
		return -1;
	}

	/* [X17: point 1]
	 * Used to parse the arguments specified by user as integers to be stored in linked list.
	 */
	err = parse_params();
	if (err)
		goto out;

	/* [X18: point 1]
	 * Used to call the (test_linked_lists) function in order to print linked list values.
	 */
	run_tests(m);
out:
	/* [X19: point 1]
	 * Used to call the (destroy_linked_list_and_free) function to free allocated memory.
	 */
	cleanup();
	return err;
}

static int proj2_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, ex4_init, NULL);
}

/*static void __exit ex4_exit(void)
{
	 [X20: point 1]
	 Exit function used to unload kernel module 
	 
	return;
}*/

/* [X21: point 1]
 * Calling the init function when loading the module
 */
static const struct proc_ops proj2_proc_fps = {
	//.proc_owner = THIS_MODULE,
	.proc_open = proj2_proc_open,
	.proc_read = seq_read,
	.proc_lseek = seq_lseek,
	.proc_release = single_release,
};

static int __init proj2_proc_init(void)
{
	proc_create("proj2", 0, NULL, &proj2_proc_fps);
	return 0;
}

static void __exit proj2_proc_exit(void)
{
	cleanup();
	remove_proc_entry("proj2", NULL);
}

module_init(proj2_proc_init);

/* [X22: point 1]
 * Calling the exit function when unloading the module
 */
module_exit(proj2_proc_exit);
