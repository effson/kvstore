#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define RED   1
#define BLACK 2

#define ENABLE_KEY_CHAR 	1

#define MAX_KEY_LEN 		256
#define MAX_VALUE_LEN 		1024

#if ENABLE_KEY_CHAR
typedef char* KEY_TYPE;
#else
typedef int KEY_TYPE;
#endif
#define RBTREE_ENTRY(name, type) \
	struct name{				 \
		struct type *right;		 \
		struct type *left; 		 \
		struct type *parent;	 \
		unsigned char color;	 \
	}

typedef struct _rbtree_node {
	KEY_TYPE key;
	void *value;

	RBTREE_ENTRY(, _rbtree_node) node;
} rbtree_node;

typedef struct _rbtree {
	struct _rbtree_node *root;
	struct _rbtree_node *nil;
} rbtree;

// 初始化树
void rbtree_init(rbtree *rbt) {
    rbtree_node *nil_node = (rbtree_node *)malloc(sizeof(rbtree_node));
    nil_node->node.color = BLACK;
    nil_node->node.left = nil_node->node.right = nil_node->node.parent = NULL;
    rbt->nil = nil_node;
    rbt->root = rbt->nil;
}


void rbtree_left_rotate(rbtree *rbt, rbtree_node *x) {

    rbtree_node *y = x->node.right;
	
    x->node.right = y->node.left;
    if (y->node.left != rbt->nil) {
        y->node.left->node.parent = x;
    }
	
    y->node.parent = x->node.parent;
    if (x->node.parent == rbt->nil) {
        rbt->root = y;
    } else if (x == x->node.parent->node.left) {
        x->node.parent->node.left = y;
    } else {
        x->node.parent->node.right = y;
    }
	
    y->node.left = x;
    x->node.parent = y;
}

// 右旋
void rbtree_right_rotate(rbtree *rbt, rbtree_node *y) {
    rbtree_node *x = y->node.left;
    y->node.left = x->node.right;
    if (x->node.right != rbt->nil) {
        x->node.right->node.parent = y;
    }
	
    x->node.parent = y->node.parent;
    if (y->node.parent == rbt->nil) {
        rbt->root = x;
    } else if (y == y->node.parent->node.right) {
        y->node.parent->node.right = x;
    } else {
        y->node.parent->node.left = x;
    }
	
    x->node.right = y;
    y->node.parent = x;
}

// 插入修复
void rbtree_insert_fixup(rbtree *rbt, rbtree_node *z) {

    while (z->node.parent->node.color == RED) {
		
        if (z->node.parent == z->node.parent->node.parent->node.left) {
            rbtree_node *y = z->node.parent->node.parent->node.right;
			
            if (y->node.color == RED) {
				
                z->node.parent->node.color = BLACK;
                y->node.color = BLACK;
                z->node.parent->node.parent->node.color = RED;
				
                z = z->node.parent->node.parent;
				
            } else {
				
                if (z == z->node.parent->node.right) {
					
                    z = z->node.parent;
                    rbtree_left_rotate(rbt, z);
				
                }
					
                z->node.parent->node.color = BLACK;
                z->node.parent->node.parent->node.color = RED;
                rbtree_right_rotate(rbt, z->node.parent->node.parent);

            }
        } else {
            rbtree_node *y = z->node.parent->node.parent->node.left;
            if (y->node.color == RED) {
				
                z->node.parent->node.color = BLACK;
                y->node.color = BLACK;
                z->node.parent->node.parent->node.color = RED;
                z = z->node.parent->node.parent;
				
            } else {
				
                if (z == z->node.parent->node.left) {
					
                    z = z->node.parent;
                    rbtree_right_rotate(rbt, z);
				
                }
				
                z->node.parent->node.color = BLACK;
                z->node.parent->node.parent->node.color = RED;
                rbtree_left_rotate(rbt, z->node.parent->node.parent);
				
            }
        }
    }
	
    rbt->root->node.color = BLACK;
}

// 插入节点
void rbtree_insert(rbtree *rbt, KEY_TYPE key, void *value) {

    rbtree_node *z = (rbtree_node *)malloc(sizeof(rbtree_node));
#if ENABLE_KEY_CHAR
	z->key = malloc(strlen(key) + 1);	// +1，给'\0'
	if (z->key == NULL) {
		fprintf(stderr, "malloc key failed!\n");
		free(z);
		return;
	}
	memset(z->key, 0, strlen(key) + 1);
	strcpy(z->key, key);   // 复制字符串到z->key
	
	z->value = malloc(strlen(value) + 1);
	if (z->value == NULL) {
		fprintf(stderr, "malloc value failed!\n");
		free(z->key);
		free(z);
		return;
	}
	memset(z->value, 0, strlen(value) + 1);
	strcpy(z->value, value);   // 复制value内容
#else
	z->key = key;
	z->value = value;
#endif

    z->node.left = rbt->nil;
	z->node.right = rbt->nil;
	z->node.parent = rbt->nil;
    z->node.color = RED;

    rbtree_node *y = rbt->nil;
    rbtree_node *x = rbt->root;

    while (x != rbt->nil) {
        y = x;
		
#if ENABLE_KEY_CHAR
		if (strcmp(z->key, x->key) < 0) {
           	x = x->node.left;
        } else if (strcmp(z->key, x->key) > 0) {
            x = x->node.right;
        } else {
			return;
		}
	
#else

        if (z->key < x->key) {
            x = x->node.left;
        } else if (z->key > x->key) {
            x = x->node.right;
        } else {
			return;
		}
#endif
    }

    z->node.parent = y;
    if (y == rbt->nil) {
        rbt->root = z;
#if ENABLE_KEY_CHAR
	} else if (strcmp(z->key, y->key) < 0) {
#else
    } else if (z->key < y->key) {
#endif
        y->node.left = z;
    } else {
        y->node.right = z;
    }

    rbtree_insert_fixup(rbt, z);
}

// 查找
rbtree_node* rbtree_search(rbtree *rbt, KEY_TYPE key) {

    rbtree_node *x = rbt->root;
#if ENABLE_KEY_CHAR
	while (x != rbt->nil && (strcmp(key, x->key) != 0)) {
        if (strcmp(key, x->key) < 0){
			x = x->node.left;
        }
        else{
            x = x->node.right;
		}
	}
#else
    while (x != rbt->nil && key != x->key) {
        if (key < x->key) {
            x = x->node.left;
        }
        else {
            x = x->node.right;
        }
    }
#endif
    return (x != rbt->nil) ? x : NULL;
}

// 最小节点
rbtree_node* rbtree_minimum(rbtree *rbt, rbtree_node *x) {
    while (x->node.left != rbt->nil) {
        x = x->node.left;
    }
    return x;
}

rbtree_node* rbtree_maximum(rbtree *rbt, rbtree_node *x) {
    while (x->node.left != rbt->nil) {
        x = x->node.right;
    }
    return x;
}

rbtree_node* rbtree_successor(rbtree *rbt, rbtree_node *x) {
	rbtree_node *y = x->node.parent;
	
    if (x->node.right != rbt->nil) {
        return rbtree_minimum(rbt, x->node.right);
    }
	while((y != rbt->nil) && (x == y->node.right)){
		x = y;
		y = y->node.parent;
    }
    return y;
}

rbtree_node* rbtree_predecessor(rbtree *rbt, rbtree_node *x) {
    if (x->node.left != rbt->nil) {
        return rbtree_maximum(rbt, x->node.left);
    }

    rbtree_node *y = x->node.parent;
    while (y != rbt->nil && x == y->node.left) {
        x = y;
        y = y->node.parent;
    }
    return y;
}

// 删除修复
void rbtree_delete_fixup(rbtree *rbt, rbtree_node *x) {
    while (x != rbt->root && x->node.color == BLACK) {
        if (x == x->node.parent->node.left) {
			
            rbtree_node *w = x->node.parent->node.right;
            if (w->node.color == RED) {
                w->node.color = BLACK;
                x->node.parent->node.color = RED;
			
                rbtree_left_rotate(rbt, x->node.parent);
                w = x->node.parent->node.right;
            }
			
            if (w->node.left->node.color == BLACK && w->node.right->node.color == BLACK) {
                w->node.color = RED;
                x = x->node.parent;
            } else {
                if (w->node.right->node.color == BLACK) {
                    w->node.left->node.color = BLACK;
                    w->node.color = RED;
				
                    rbtree_right_rotate(rbt, w);
                    w = x->node.parent->node.right;
                }
				
                w->node.color = x->node.parent->node.color;
                x->node.parent->node.color = BLACK;
                w->node.right->node.color = BLACK;
                rbtree_left_rotate(rbt, x->node.parent);
				
                x = rbt->root;
            }
        } else {
            rbtree_node *w = x->node.parent->node.left;
            if (w->node.color == RED) {
                w->node.color = BLACK;
                x->node.parent->node.color = RED;
                rbtree_right_rotate(rbt, x->node.parent);
                w = x->node.parent->node.left;
            }
            if (w->node.right->node.color == BLACK && w->node.left->node.color == BLACK) {
                w->node.color = RED;
                x = x->node.parent;
            } else {
                if (w->node.left->node.color == BLACK) {
                    w->node.right->node.color = BLACK;
                    w->node.color = RED;
                    rbtree_left_rotate(rbt, w);
                    w = x->node.parent->node.left;
                }
                w->node.color = x->node.parent->node.color;
                x->node.parent->node.color = BLACK;
                w->node.left->node.color = BLACK;
                rbtree_right_rotate(rbt, x->node.parent);
				
                x = rbt->root;
            }
        }
    }
    x->node.color = BLACK;
}

// 删除
void rbtree_delete(rbtree *rbt, rbtree_node *z) {
    rbtree_node *y; // 实际被删或者被移动的位置
    rbtree_node *x; // y的子节点，用来调整树
    unsigned char y_original_color;

    if (z->node.left == rbt->nil || z->node.right == rbt->nil) {
        y = z;
    } else {
        y = rbtree_successor(rbt, z);
    }

    y_original_color = y->node.color;

    if (y->node.left != rbt->nil) {   // x是y的唯一子节点（可能是nil）
        x = y->node.left;
    } else {
        x = y->node.right;
    }

    x->node.parent = y->node.parent;

    if (y->node.parent == rbt->nil) {
        rbt->root = x;
    } else if (y == y->node.parent->node.left) {
        y->node.parent->node.left = x;
    } else {
        y->node.parent->node.right = x;
    }

    if (y != z) {
#if ENABLE_KEY_CHAR
		KEY_TYPE ktmp = z->key;
		z->key = y->key;
		y->key = ktmp;
		
		void *vtmp = z->value;
		z->value = y->value;
		y->value = vtmp;
#else
        z->key = y->key;
        z->value = y->value;
#endif
    }

    if (y_original_color == BLACK) {
        rbtree_delete_fixup(rbt, x);
    }
#if ENABLE_KEY_CHAR
	free(y->key);	 // 释放 key
	free(y->value);  // 释放 value
#endif

    free(y);
}

void rbtree_inorder(rbtree *rbt, rbtree_node *node) {
	if (node != rbt->nil) {
		rbtree_inorder(rbt, node->node.left);
#if ENABLE_KEY_CHAR
		printf("key:%s, value:%s  ", node->key, node->value);
#else
		printf("key : %d", node->key);
#endif
		rbtree_inorder(rbt, node->node.right);
	}
}

int main(){
#if ENABLE_KEY_CHAR
	KEY_TYPE KeyArray[20] = {"afsd","shg","hgs","yi","rbd","rts","qda","jeff","qinl","ziy",
								"jijnj","grh","jmh","cjr","qon","zj","erh","fgj","yso","sdg"};
	char *ValueArray[20] = {"Afsd","Shg","Hgs","Yi","Rbd","Rts","Qda","Jeff","Qinl","Ziy",
								"Jijnj","Grh","Jmh","Cjr","Qon","Zj","Erh","Fgj","Yso","Sdg"};

#else
	int KeyArray[20] = {24, 25, 13, 35, 23, 26, 67, 47, 38, 98, 10, 19, 17, 49, 12, 21, 9, 18, 14, 15};
#endif
	rbtree *rbt = (rbtree *)malloc(sizeof(rbtree));

	if (rbt == NULL) {
		printf("malloc failed\n");
		return -1;
	}
	rbtree_init(rbt);
	int i = 0;
	for (i = 0;i < 20;i ++) {
#if ENABLE_KEY_CHAR
		rbtree_insert(rbt, KeyArray[i], ValueArray[i]);
#else
		rbtree_insert(rbt, KeyArray[i], NULL);
#endif
	}
	rbtree_inorder(rbt, rbt->root);
	printf("--------------\n");

	for (i = 0;i < 20;i ++) {
		rbtree_node *node = rbtree_search(rbt, KeyArray[i]);
		rbtree_delete(rbt, node);
	
		rbtree_inorder(rbt, rbt->root);
		printf("--------------\n");
	}
}
